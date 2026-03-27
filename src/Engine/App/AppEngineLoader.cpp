#include "Engine/App/AppEngineLoader.hpp"
#include "Engine/App/AppEngineСonfiguration.hpp"
#include "Engine/Tools/BybitRestAPI.hpp"
#include "Engine/Tools/DBHash.hpp"
#include <QUrlQuery>
#include <QDebug>

namespace Engine {

    AppEngineLoader::AppEngineLoader(QObject* parent) : QObject(parent) {}

    AppEngineLoader::~AppEngineLoader() {
        cancelCurrentRequest();
    }

    void AppEngineLoader::startLoading() {

        QMutexLocker locker(&m_mutex);
        if (m_loading) {
            emit errorOccurred("Loading already in progress");
            return;
        }

        m_tradingPairs.clear();
        m_loading = true;

        m_loadSteps.append({"Loading the platform configuration", [this]() { loadConfigAsync(); }});
        
        DBHash db;
        if (db.dbExist()) {
            m_loadSteps.append({"Loading from database", [this]() { loadFromDatabaseAsync(); }});
        } else {
            m_loadSteps.append({"Loading SPOT pairs", [this]() { requestTradingPairsAsync(TypesTrade::SPOT); }});
            m_loadSteps.append({"Loading LINEAR pairs", [this]() { requestTradingPairsAsync(TypesTrade::LINEAR); }});
            m_loadSteps.append({"Loading INVERSE pairs", [this]() { requestTradingPairsAsync(TypesTrade::INVERSE); }});
            m_loadSteps.append({"Loading OPTION pairs", [this]() { requestTradingPairsAsync(TypesTrade::OPTION); }});
            m_loadSteps.append({"Saving to database", [this]() { saveToDatabaseAsync(); }});
        }
        
        m_totalSteps = m_loadSteps.size();
        m_currentStep = 0;
        locker.unlock();

        execStep();
    }

    void AppEngineLoader::execStep() {
        if (m_currentStep >= m_loadSteps.size()) {
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

    void AppEngineLoader::loadConfigAsync() {
        loadConfig();
        execNextStep();
    }

    void AppEngineLoader::loadConfig() {
        auto& engine_config = AppEngineСonfiguration::instance();

        if (!engine_config.readСonfiguration()) {
            emit errorOccurred(engine_config.getLastError());
            return;
        }

        auto& config = engine_config.getСonfiguration();

        if (config.m_api.isEmpty()) {
            emit errorOccurred("No API configuration found");
            return;
        }

        auto api = config.m_api.values()[0];
        m_apiKey = api.api_key;
        m_secretKey = api.secret_key;
        m_testnet = api.testnet;

        if (m_apiKey.isEmpty()) {
            emit errorOccurred("API key is empty");
            return;
        }

        if (m_secretKey.isEmpty()) {
            emit errorOccurred("Secret key is empty");
            return;
        }
    }

    void AppEngineLoader::loadFromDatabaseAsync() {
        loadFromDatabase();
        execNextStep();
    }

    void AppEngineLoader::loadFromDatabase() {
        DBHash db;
        if (!db.dbExist()) {
            emit errorOccurred("Database does not exist");
            return;
        }

        QMutexLocker locker(&m_mutex);
        if (!db.getAllItems(m_tradingPairs)) {
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
        for (const auto& items : m_tradingPairs) {
            for (const auto& item : items) {
                if (!db.addItem(item)) {
                    emit errorOccurred(db.error());
                    return;
                }
            }
        }

    }

    void AppEngineLoader::requestTradingPairsAsync(TypesTrade trade) {
        cancelCurrentRequest();

        m_currentApi = new BybitRestAPI(m_apiKey, m_secretKey, m_testnet);

        // Подключаем сигналы
        connect(m_currentApi, &BybitRestAPI::dataReceived, this,
                [this, trade](const QJsonObject& data) {
                    if (m_currentApi == sender()) {
                        cancelCurrentRequest();
                        processSymbols(data);
                        execNextStep();
                    }
                });

        connect(m_currentApi, &BybitRestAPI::errorOccurred, this,
                [this, trade](const QString& error) {
                    if (m_currentApi == sender()) {
                        cancelCurrentRequest();
                        emit errorOccurred(QString("Error loading %1: %2").arg(tradeToString(trade)).arg(error));
                        execNextStep();
                    }
                });

        // Выполняем запрос
        QUrlQuery params;
        params.addQueryItem("category", tradeToString(trade).toLower());
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

        {
            QMutexLocker locker(&m_mutex);
            m_tradingPairs.insert(category, lst);
        }
    }

    QList<TradingInfo> AppEngineLoader::getData(const QString& category) const {
        QMutexLocker locker(&m_mutex);
        return m_tradingPairs.value(category);
    }

} // namespace Engine