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

    const int LOADING_TIMEOUT = 30000; // 30 секунд

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

        m_loadSteps = {
            { "Loading the platform configuration", [this]() { return loadConfigSync(); }},
            { "Loading SPOT pairs", [this]() { return loadTradingPairsSync(TMarket::SPOT); }},
            { "Loading LINEAR pairs", [this]() { return loadTradingPairsSync(TMarket::LINEAR); }},
            { "Loading INVERSE pairs", [this]() { return loadTradingPairsSync(TMarket::INVERSE); }},
            { "Loading OPTION pairs", [this]() { return loadTradingPairsSync(TMarket::OPTION); }}
        };

        m_progressTotal = m_loadSteps.size();

        QFuture<bool> future = QtConcurrent::run([this]() {
            return runLoadingThread();
        });
        
        m_futureWatcher->setFuture(future);
    }

    bool AppEngineLoader::runLoadingThread() {
        for (int i = 0; i < m_loadSteps.size(); ++i) {
            if (QThread::currentThread()->isInterruptionRequested()) {
                handleError("Loading was cancelled");
                return false;
            }

            emitStepStarted(m_loadSteps[i].name);
            
            bool stepSuccess = m_loadSteps[i].action();
            
            if (!stepSuccess) 
                return false;

            emitProgress(i + 1, m_loadSteps.size());
        }

        if (!saveToDatabase())
            return false;

        return true;
    }

    bool AppEngineLoader::loadConfigSync() {
        if (!Settings::readAllConfig()) {
            handleError(Settings::getLastError());
            return false;
        }

        auto config = PlatformConfig::instance().getConfig();
        
        if (config.m_api.isEmpty()) {
            handleError("No API configuration found");
            return false;
        }

        m_api = config.m_api.values()[0];
    
        if (m_api.api_key.isEmpty()) {
            handleError("API key is empty");
            return false;
        }

        return true;
    }

    bool AppEngineLoader::loadTradingPairsSync(TMarket market) {
        QEventLoop loop;
        bool success = false;
        bool timeout = false;
        QString marketStr = marketToString(market);

        QTimer* timer = new QTimer();
        timer->setSingleShot(true);
        timer->setInterval(LOADING_TIMEOUT);
        
        auto timeoutConn = connect(timer, &QTimer::timeout, this, [&, timer]() {
            if (!success && !timeout) {
                timeout = true;
                handleError(QString("Timeout loading %1 pairs").arg(marketStr));
                
                timer->stop();
                timer->deleteLater();
                
                if (loop.isRunning())
                    QMetaObject::invokeMethod(&loop, "quit", Qt::QueuedConnection);
            }
        });

        auto readyConn = connect(this, &AppEngineLoader::tradingPairsReady, this, [&, timer](TMarket loadedMarket) {
            if (loadedMarket == market) {
                success = true;
                
                if (timer) {
                    timer->stop();
                    timer->deleteLater();
                }
                
                if (loop.isRunning())
                    QMetaObject::invokeMethod(&loop, "quit", Qt::QueuedConnection);
            }
        });

        auto errorConn = connect(this, &AppEngineLoader::errorEngine, this, [&, timer](const QString& error) {
            if (!timeout)
                handleError(QString("Error loading %1: %2").arg(marketStr).arg(error));
            
            if (timer) {
                timer->stop();
                timer->deleteLater();
            }
            
            if (loop.isRunning())
                QMetaObject::invokeMethod(&loop, "quit", Qt::QueuedConnection);
        });

        timer->start();
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
            handleError(error);
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

    bool AppEngineLoader::saveToDatabase() {
        DBHash db;
        
        if (db.dbExist()) {
            qInfo() << "Loading from database";
            QMutexLocker locker(&m_mutex);
            if (!db.getAllItems(m_tradingPairs)) {
                handleError(db.error());
                return false;
            }
            qInfo() << "Loading from the database is completed";
        } else {
            qInfo() << "Creating a database";
            if (!db.create()) {
                handleError(db.error());
                return false;
            }

            QMutexLocker locker(&m_mutex);
            for (const auto& items : m_tradingPairs) {
                for (const auto& item : items) {
                    if (!db.addItem(item)) {
                        handleError(db.error());
                        return false;
                    }
                }
            }
            qInfo() << "Database ready";
        }

        return true;
    }

    void AppEngineLoader::handleError(const QString& error) {
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