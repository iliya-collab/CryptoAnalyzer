#include "Engine/App/AppEngineLoader.hpp"
#include "Engine/BybitRestAPI.hpp"
#include "Engine/DBHash.hpp"
#include "Managers/Settings.hpp"
#include "Configs/PlatformConfig.hpp"

#include <QEventLoop>
#include <QUrlQuery>
#include <QTimer>
#include <QThread>
#include <QDebug>

namespace Engine {

    const int LOADING_TIMEOUT = 30000;

    AppEngineLoader::AppEngineLoader(QObject* parent) : QObject(parent), m_futureWatcher(new QFutureWatcher<bool>(this)) {
        connect(m_futureWatcher, &QFutureWatcher<bool>::finished, this, [this]() {
            bool success = m_futureWatcher->result();
            emit finished(success);
        });
    }

    AppEngineLoader::~AppEngineLoader() {
        if (m_futureWatcher && m_futureWatcher->isRunning()) {
            m_futureWatcher->cancel();
            m_futureWatcher->waitForFinished();
        }
    }

    void AppEngineLoader::startLoading() {
        if (m_futureWatcher && m_futureWatcher->isRunning()) {
            emit errorEngine("Loading already in progress");
            return;
        }

        {
            QMutexLocker locker(&m_mutex);
            m_tradingPairs.clear();
        }

        QFuture<bool> future = QtConcurrent::run([this]() {
            return runLoadingThread();
        });
        
        m_futureWatcher->setFuture(future);
    }

    bool AppEngineLoader::runLoadingThread() {
        // Базовые шаги
        m_loadSteps.clear();
        
        m_loadSteps.append({"Loading the platform configuration", [this]() { return loadConfigSync(); }});
        
        DBHash db;

        if (db.dbExist())
            // Если БД существует - добавляем только шаги работы с БД
            m_loadSteps.append({"Loading from database", [this]() { return loadFromDatabase(); }});
        else {
            // Если БД нет - добавляем шаги загрузки из сети
            m_loadSteps.append({"Loading SPOT pairs", [this]() { return loadTradingPairsSync(TMarket::SPOT); }});
            m_loadSteps.append({"Loading LINEAR pairs", [this]() { return loadTradingPairsSync(TMarket::LINEAR); }});
            m_loadSteps.append({"Loading INVERSE pairs", [this]() { return loadTradingPairsSync(TMarket::INVERSE); }});
            m_loadSteps.append({"Loading OPTION pairs", [this]() { return loadTradingPairsSync(TMarket::OPTION); }});
            m_loadSteps.append({"Saving to database", [this]() { return saveToDatabase(); }});
        }
        
        for (int i = 0; i < m_loadSteps.size(); ++i) {
            if (QThread::currentThread()->isInterruptionRequested()) {
                emitError("Loading was cancelled");
                return false;
            }

            emitStepStarted(m_loadSteps[i].name);
            
            if (!m_loadSteps[i].action())
                return false;
            
            emitProgress(i + 1, m_loadSteps.size());
        }

        return true;
    }

    bool AppEngineLoader::loadConfigSync() {
        if (!Settings::readAllConfig()) {
            emitError(Settings::getLastError());
            return false;
        }

        auto config = PlatformConfig::instance().getConfig();
        
        if (config.m_api.isEmpty()) {
            emitError("No API configuration found");
            return false;
        }

        m_api = config.m_api.values()[0];
    
        if (m_api.api_key.isEmpty()) {
            emitError("API key is empty");
            return false;
        }

        return true;
    }

    bool AppEngineLoader::loadTradingPairsSync(TMarket market) {
        QEventLoop loop;
        bool success = false;
        bool timeout = false;
        QString marketStr = marketToString(market);

        QTimer timer;
        timer.setSingleShot(true);
        timer.setInterval(LOADING_TIMEOUT);

        auto stopTimer = [&timer]() {
            if (timer.isActive()) {
                QMetaObject::invokeMethod(&timer, "stop", Qt::QueuedConnection);
            }
        };
        
        auto timeoutConn = connect(&timer, &QTimer::timeout, this, [&]() {
            if (!success && !timeout) {
                timeout = true;

                emitError(QString("Timeout loading %1 pairs").arg(marketStr));
                
                if (loop.isRunning())
                    QMetaObject::invokeMethod(&loop, "quit", Qt::QueuedConnection);
            }
        }, Qt::DirectConnection);

        auto readyConn = connect(this, &AppEngineLoader::tradingPairsReady, this, [&](TMarket loadedMarket) {
            if (loadedMarket == market) {
                success = true;

                stopTimer();
                
                if (loop.isRunning())
                    QMetaObject::invokeMethod(&loop, "quit", Qt::QueuedConnection);
            }
        }, Qt::QueuedConnection);

        auto errorConn = connect(this, &AppEngineLoader::errorEngine, this, [&](const QString& error) {
            if (!timeout)
                emitError(QString("Error loading %1: %2").arg(marketStr).arg(error));
            
            stopTimer();
            
            if (loop.isRunning())
                QMetaObject::invokeMethod(&loop, "quit", Qt::QueuedConnection);
        }, Qt::QueuedConnection);

        timer.start();
        getTradingPairs(market);
        loop.exec();

        disconnect(timeoutConn);
        disconnect(readyConn);
        disconnect(errorConn);

        return success && !timeout;
    }

    void AppEngineLoader::getTradingPairs(TMarket market) {
        BybitRestAPI* bybit_api = new BybitRestAPI(m_api);

        connect(bybit_api, &BybitRestAPI::dataReceived, this, [this, bybit_api, market](const QJsonObject& data) {
            processSymbols(data);
            emit tradingPairsReady(market);
            bybit_api->deleteLater();
        }, Qt::QueuedConnection);

        connect(bybit_api, &BybitRestAPI::errorOccurred, this, [this, bybit_api](const QString& error) {
            emitError(error);
            bybit_api->deleteLater();
        }, Qt::QueuedConnection);

        QUrlQuery params;
        params.addQueryItem("category", marketToString(market).toLower());
        bybit_api->requestEndpoint("/v5/market/instruments-info", params, LOADING_TIMEOUT);
    }

    void AppEngineLoader::processSymbols(const QJsonObject& data) {
        QJsonObject result = data["result"].toObject();
        QString category = result["category"].toString();
        QJsonArray list = result["list"].toArray();

        QList<TradingInfo> lst;

        for (const auto& obj : list) {
            QJsonObject item = obj.toObject();

            TradingInfo info;
            info.category = category;
            info.symbol = item["symbol"].toString();
            info.base_coin = item["baseCoin"].toString();
            info.quote_coin = item["quoteCoin"].toString();

            lst.append(info);
        }

        {
            QMutexLocker locker(&m_mutex);
            m_tradingPairs.insert(category, lst);
        }

        qInfo().noquote() << QString("Loaded %1 %2 pairs").arg(lst.size()).arg(category);
    }

    bool AppEngineLoader::loadFromDatabase() {
        DBHash db;

        if (!db.dbExist())
            return false;

        QMutexLocker locker(&m_mutex);
        if (!db.getAllItems(m_tradingPairs)) {
            emitError(db.error());
            return false;
        }

        return true;
    }

    bool AppEngineLoader::saveToDatabase() {
        DBHash db;
        
        if (!db.create()) {
            emitError(db.error());
            return false;
        }

        QMutexLocker locker(&m_mutex);
        for (const auto& items : m_tradingPairs) {
            for (const auto& item : items) {
                if (!db.addItem(item)) {
                    emitError(db.error());
                    return false;
                }
            }
        }

        return true;
    }

    void AppEngineLoader::emitError(const QString& error) {
        QMetaObject::invokeMethod(this, [this, error]() {
            emit errorEngine(error);
        }, Qt::QueuedConnection);
    }

    void AppEngineLoader::emitProgress(int current, int total) {
        QMetaObject::invokeMethod(this, [this, current, total]() {
            emit progressChanged(current, total);
        }, Qt::QueuedConnection);
    }

    void AppEngineLoader::emitStepStarted(const QString& step) {
        QMetaObject::invokeMethod(this, [this, step]() {
            emit stepStarted(step);
        }, Qt::QueuedConnection);
    }

    QList<TradingInfo> AppEngineLoader::getData(const QString& category) const {
        QMutexLocker locker(&m_mutex);
        return m_tradingPairs.value(category);
    }

}