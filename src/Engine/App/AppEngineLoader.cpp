#include "Engine/App/AppEngineLoader.hpp"
#include "Engine/Tools/BybitRestAPI.hpp"
#include "Engine/Tools/DBHash.hpp"

#include <QUrlQuery>
#include <QDebug>
#include <QThread>

namespace Engine {

    AppEngineLoader::AppEngineLoader(QObject* parent) : QObject(parent) {
        qDebug() << Q_FUNC_INFO << "created in:" << QThread::currentThread();
        m_currentApi = new BybitRestAPI(this);
    }

    AppEngineLoader::~AppEngineLoader() {
        qDebug() << Q_FUNC_INFO << "launched from:" << QThread::currentThread();
        //cancelCurrentRequest();
        qDebug() << Q_FUNC_INFO << "finished";
    }

    void AppEngineLoader::execStep() {
        if (m_totalSteps == 0)
            emit progressChanged("Database exists", 1, 1);

        if (m_currentStep >= m_totalSteps) {
            emit finished(true);
            return;
        }
        
        auto [name, action] = m_loadSteps.value(m_currentStep);
        emit progressChanged(name, m_currentStep + 1, m_totalSteps);
        action();
    }

    void AppEngineLoader::execNextStep() {
        m_currentStep++;
        execStep();
    }

    void AppEngineLoader::loadFromDatabase(const QString& category) {
        DBHash db;
        if (!db.dbExist()) {
            emit errorOccurred("Database does not exist");
            return;
        }

        QMutexLocker locker(&m_mutex);
        m_tradingPairs.clear();
        if (!db.getAllItems(category, m_tradingPairs)) {
            emit errorOccurred(db.error());
            return;
        }
    }

    void AppEngineLoader::saveToDatabaseAsync() {
        saveToDatabase();
        execNextStep();
    }

    void AppEngineLoader::saveToDatabase() {
        DBHash db;
        if (!db.create()) {
            emit errorOccurred(db.error());
            return;
        }

        QMutexLocker locker(&m_mutex);
        for (const auto& item : m_tradingPairs) {
            if (!db.addItem(item)) {
                emit errorOccurred(db.error());
                return;
            }
        }

    }

    void AppEngineLoader::requestTradingPairsAsync(const QString& category) {
        // Подключаем сигналы
        connect(m_currentApi, &BybitRestAPI::dataReceived, this, [this](const QJsonObject& data) {
            processSymbols(data);
            execNextStep();
        });

        connect(m_currentApi, &BybitRestAPI::errorOccurred, this, [this](const QString& error) {
            emit errorOccurred(QString("Error loading: %2").arg(error));
            execNextStep();
        });

        // Выполняем запрос
        QUrlQuery params;
        params.addQueryItem("category", category);
        m_currentApi->requestEndpoint("/v5/market/instruments-info", params, LOADING_TIMEOUT);
    }

    void AppEngineLoader::cancelCurrentRequest() {
        if (m_currentApi) {
            disconnect(m_currentApi, nullptr, this, nullptr);
            m_currentApi->deleteLater();
            m_currentApi = nullptr;
        }
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

        QMutexLocker locker(&m_mutex);
        m_tradingPairs.append(lst);
    }

    void AppEngineLoader::startLoading() {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        QMutexLocker locker(&m_mutex);

        if (m_loading) {
            emit errorOccurred("Loading already in progress");
            return;
        }

        m_tradingPairs.clear();
        m_loadSteps.clear();
        m_loading = true;

        DBHash db;
        if (!db.dbExist()) {
            m_loadSteps.append({"Loading SPOT pairs", [this]() { requestTradingPairsAsync("spot"); }});
            m_loadSteps.append({"Loading LINEAR pairs", [this]() { requestTradingPairsAsync("linear"); }});
            m_loadSteps.append({"Loading INVERSE pairs", [this]() { requestTradingPairsAsync("inverse"); }});
            m_loadSteps.append({"Loading OPTION pairs", [this]() { requestTradingPairsAsync("option"); }});
            m_loadSteps.append({"Saving to database", [this]() { saveToDatabaseAsync(); }});
        }

        m_totalSteps = m_loadSteps.size();
        m_currentStep = -1;

        locker.unlock();

        execNextStep();
    }

    void AppEngineLoader::setAPI(const API& api) {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        if (!m_currentApi)
            return;

        m_currentApi->initAPI(api);
    }

    QList<TradingInfo> AppEngineLoader::getData(const QString& category) {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        loadFromDatabase(category);
        return m_tradingPairs;
    }

} // namespace Engine