#include "Engine/App/AppEngineLoader.hpp"

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
        if (m_totalSteps == 0) {
            m_loading = false;
            emit progressChanged("Database exists", 1, 1);
            emit finished(true);
            return;
        }

        if (m_currentStep >= m_totalSteps) {
            m_loading = false;
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
        if (!m_dbSystem.dbExist(m_dbSystem.m_dbCrypto)) {
            emit errorOccurred("Database does not exist");
            return;
        }

        QMutexLocker locker(&m_mutex);
        m_tradingPairs.clear();
        if (category == "ALL") {
            if (!m_dbSystem.getAllItems(m_tradingPairs)) {
                emit errorOccurred(m_dbSystem.error());
                return;
            }
        }
        else if (!m_dbSystem.getItems(m_tradingPairs, category)) {
            emit errorOccurred(m_dbSystem.error());
            return;
        }
    }

    void AppEngineLoader::saveToDatabaseAsync() {
        saveToDatabase();
        QMutexLocker locker(&m_mutex);
        m_tradingPairs.clear();
        locker.unlock();
        execNextStep();
    }

    void AppEngineLoader::saveToDatabase() {
        if (!m_dbSystem.createCryptoDB()) {
            emit errorOccurred(m_dbSystem.error());
            return;
        }

        QMutexLocker locker(&m_mutex);
        if (!m_dbSystem.addItems(m_tradingPairs)) {
            emit errorOccurred(m_dbSystem.error());
            return;
        }

    }

    void AppEngineLoader::requestTradingPairsAsync() {
        if (!m_currentApi)
            return;

        connect(m_currentApi, &BybitRestAPI::dataReceived, this, [this](const QJsonObject& data) {
            processSymbols(data);
            execNextStep();
        }, Qt::SingleShotConnection);

        connect(m_currentApi, &BybitRestAPI::errorOccurred, this, [this](const QString& error) {
            emit errorOccurred(QString("Error loading: %2").arg(error));
            execNextStep();
        }, Qt::SingleShotConnection);

        // Выполняем запрос
        QUrlQuery params;
        params.addQueryItem("category", "spot");
        m_currentApi->requestEndpoint("/v5/market/instruments-info", params, LOADING_TIMEOUT);
    }

    void AppEngineLoader::requestInfoAboutAccount() {
        if (!m_currentApi)
            return;

        connect(m_currentApi, &BybitRestAPI::dataReceived, this, [this](const QJsonObject& data) {
            emit apiChecked(data["retMsg"] == "OK");
        }, Qt::SingleShotConnection);

        connect(m_currentApi, &BybitRestAPI::errorOccurred, this, [this](const QString& error) {
            emit apiChecked(false);
        }, Qt::SingleShotConnection);

        m_currentApi->requestEndpoint("/v5/account/info", QUrlQuery(), LOADING_TIMEOUT);
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

        if (!m_dbSystem.dbExist(m_dbSystem.m_dbCrypto)) {
            m_loadSteps.append({"Loading spot pairs", [this]() { requestTradingPairsAsync(); }});
            m_loadSteps.append({"Saving spot pairs to database", [this]() { saveToDatabaseAsync(); }});
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

    void AppEngineLoader::checkAPI() {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        requestInfoAboutAccount();
    }

    QList<TradingInfo> AppEngineLoader::loadTradingPairs(const QString& category) {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        loadFromDatabase(category);
        return m_tradingPairs;
    }

} // namespace Engine