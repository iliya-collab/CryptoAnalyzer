#include "AppEngineLoader.hpp"
#include "Tools/Database/SqliteDatabaseManager.hpp"
#include <QUrlQuery>
#include <QDebug>
#include <QThread>

namespace Engine {

    AppEngineLoader::AppEngineLoader(QObject* parent) : QObject(parent) {
        qDebug() << Q_FUNC_INFO << "created in:" << QThread::currentThread();
        m_currentApi = new BybitRestAPI(this);
        connect(m_currentApi, &BybitRestAPI::downloadProgress, this, &AppEngineLoader::downloadProgress, Qt::UniqueConnection);
    }

    AppEngineLoader::~AppEngineLoader() {
        qDebug() << Q_FUNC_INFO << "launched from:" << QThread::currentThread();
    }

    void AppEngineLoader::init() {

        const QString dbFile = "db/crypto.db";

        auto& manager = SqliteDatabaseManager::instance();

        if (!manager.open(dbFile))
            emit errorOccurred(manager.error());

        m_cryptoRep = std::move(RepositoryCreater::instance().createCryptoRepository(dbFile, manager));
        m_candleRep = std::move(RepositoryCreater::instance().createCandleRepository(dbFile, manager));

        if (!m_cryptoRep->init())
            emit errorOccurred(m_cryptoRep->error());

        if (!m_candleRep->init())
            emit errorOccurred(m_cryptoRep->error());
    }

    TradeList AppEngineLoader::loadAllFromCryptoRepository() {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        QMutexLocker locker(&m_mutex);
        if (!m_cryptoRep->selectTrades()) {
            emit errorOccurred(m_cryptoRep->error());
            return TradeList();
        }
        return m_cryptoRep->getSelectedData();
    }

    TradeList AppEngineLoader::loadFromCryptoRepository(const QString& quoteCoin) {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        QMutexLocker locker(&m_mutex);
        if (!m_cryptoRep->selectTrades(quoteCoin)) {
            emit errorOccurred(m_cryptoRep->error());
            return TradeList();
        }
        return m_cryptoRep->getSelectedData();
    }

    void AppEngineLoader::clearCryptoRepository() {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        QMutexLocker locker(&m_mutex);
        if (!m_cryptoRep->clear()) {
            emit errorOccurred(m_cryptoRep->error());
            return;
        }
    }

    void AppEngineLoader::saveToCryptoRepository(const TradeList& tradePairs) {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        QMutexLocker locker(&m_mutex);
        if (!m_cryptoRep->insertTrades(tradePairs)) {
            emit errorOccurred(m_cryptoRep->error());
            return;
        }
    }

    void AppEngineLoader::saveToCandleRepository(const ItemCandle& newCandle) {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        QMutexLocker locker(&m_mutex);
        if (!m_candleRep->insertCandle(newCandle))
            emit errorOccurred(m_candleRep->error());
    }

    void AppEngineLoader::saveToCandlesRepository(const CandleList& newCandles) {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        QMutexLocker locker(&m_mutex);
        if (!m_candleRep->insertCandles(newCandles))
            emit errorOccurred(m_candleRep->error());
    }

    void AppEngineLoader::requestTradePairs() {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        if (!m_currentApi)
            return;

        emit messageSent("Loading spot pairs...");

        connect(m_currentApi, &BybitRestAPI::dataReceived, this, [this](const QJsonObject& data) {
            TradeList pairs;
            processRequestTradePairs(pairs, data);
            clearCryptoRepository();
            saveToCryptoRepository(pairs);
            emit tradePairsReceived(pairs);
            emit messageSent("Spot pairs loaded");
        }, Qt::SingleShotConnection);

        connect(m_currentApi, &BybitRestAPI::errorOccurred, this, &AppEngineLoader::errorOccurred, Qt::SingleShotConnection);

        QUrlQuery params;
        params.addQueryItem("category", "spot");
        m_currentApi->requestEndpoint("/v5/market/instruments-info", params, LOADING_TIMEOUT);
    }

    void AppEngineLoader::requestCandles(const QString& symbol, const QString& interval, int start, int end) {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        if (!m_currentApi)
            return;

        emit messageSent("Loading candles...");

        connect(m_currentApi, &BybitRestAPI::dataReceived, this, [this](const QJsonObject& data) {
            CandleList candles;
            processRequestCandles(candles, data);
            saveToCandlesRepository(candles);
            emit candlesReceived(candles);
            emit messageSent("Candles loaded");
        }, Qt::SingleShotConnection);

        connect(m_currentApi, &BybitRestAPI::errorOccurred, this, &AppEngineLoader::errorOccurred, Qt::SingleShotConnection);

        QUrlQuery params;
        params.addQueryItem("category", "spot");
        params.addQueryItem("symbol", symbol);
        params.addQueryItem("interval", interval);
        params.addQueryItem("start", QString::number(start));
        params.addQueryItem("end", QString::number(end));
        params.addQueryItem("limit", "1000");
        m_currentApi->requestEndpoint("/v5/market/kline", params, LOADING_TIMEOUT);
    }

    void AppEngineLoader::requestInfoAboutAccount() {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        if (!m_currentApi)
            return;

        emit messageSent("Getting account information...");

        connect(m_currentApi, &BybitRestAPI::dataReceived, this, [this](const QJsonObject& data) {
            emit infoAboutAccountReceived(data["retMsg"] == "OK");
            emit messageSent("Account information received");
        }, Qt::SingleShotConnection);

        connect(m_currentApi, &BybitRestAPI::errorOccurred, this, &AppEngineLoader::errorOccurred, Qt::SingleShotConnection);

        m_currentApi->requestEndpoint("/v5/account/info", QUrlQuery(), LOADING_TIMEOUT);
    }

    void AppEngineLoader::processRequestCandles(CandleList& candles, const QJsonObject& data) {
        QJsonObject result = data["result"].toObject();
        QString category = result["category"].toString();
        QString symbol = result["symbol"].toString();
        QJsonArray list = result["list"].toArray();

        QMutexLocker locker(&m_mutex);
        for (const auto& obj : list) {
            QJsonObject item = obj.toObject();
            QJsonArray itemArr = obj.toArray();
            ItemCandle candle;
            candle.m_open = itemArr[1].toString().toDouble();
            candle.m_high = itemArr[2].toString().toDouble();
            candle.m_low = itemArr[3].toString().toDouble();
            candle.m_close = itemArr[4].toString().toDouble();
            candle.m_confirm = true;
            candles.append(candle);
        }
    }

    void AppEngineLoader::processRequestTradePairs(TradeList& pairs, const QJsonObject& data) {
        QJsonObject result = data["result"].toObject();
        QString category = result["category"].toString();
        QJsonArray list = result["list"].toArray();

        QMutexLocker locker(&m_mutex);
        for (const auto& obj : list) {
            QJsonObject item = obj.toObject();
            ItemTrade info;
            info.symbol = item["symbol"].toString();
            info.base_coin = item["baseCoin"].toString();
            info.quote_coin = item["quoteCoin"].toString();
            pairs.append(info);
        }
    }

    void AppEngineLoader::setAPI(const API& api) {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        if (!m_currentApi)
            return;

        m_currentApi->initAPI(api);
    }

} // namespace Engine