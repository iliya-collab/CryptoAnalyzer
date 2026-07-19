#include "MarketDataManager.hpp"
#include "Tools/Database/SqliteDatabaseManager.hpp"
#include <QUrlQuery>
#include <QDebug>
#include <QThread>

namespace Core {

    MarketDataManager::MarketDataManager(QObject* parent) : QObject(parent) {
        qDebug() << Q_FUNC_INFO << "created in:" << QThread::currentThread();
        m_currentApi = new Tools::BybitRestAPI(this);
        connect(m_currentApi, &Tools::BybitRestAPI::downloadProgress, this, &MarketDataManager::downloadProgress, Qt::UniqueConnection);
    }

    MarketDataManager::~MarketDataManager() {
        qDebug() << Q_FUNC_INFO << "launched from:" << QThread::currentThread();
    }

    void MarketDataManager::init() {

        const QString dbFile = "db/crypto.db";

        auto& manager = Tools::SqliteDatabaseManager::instance();

        if (!manager.open(dbFile))
            emit errorOccurred(manager.error());

        m_cryptoRep = std::move(Tools::RepositoryCreater::instance().createCryptoRepository(dbFile, manager));
        m_candleRep = std::move(Tools::RepositoryCreater::instance().createCandleRepository(dbFile, manager));

        if (!m_cryptoRep->init())
            emit errorOccurred(m_cryptoRep->error());

        if (!m_candleRep->init())
            emit errorOccurred(m_cryptoRep->error());
    }

    TradeList MarketDataManager::loadAllFromCryptoRepository() {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        QMutexLocker locker(&m_mutex);
        if (!m_cryptoRep->selectTrades()) {
            emit errorOccurred(m_cryptoRep->error());
            return TradeList();
        }
        return m_cryptoRep->getSelectedData();
    }

    TradeList MarketDataManager::loadFromCryptoRepository(const QString& quoteCoin) {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        QMutexLocker locker(&m_mutex);
        if (!m_cryptoRep->selectTrades(quoteCoin)) {
            emit errorOccurred(m_cryptoRep->error());
            return TradeList();
        }
        return m_cryptoRep->getSelectedData();
    }

    void MarketDataManager::clearCryptoRepository() {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        QMutexLocker locker(&m_mutex);
        if (!m_cryptoRep->clear()) {
            emit errorOccurred(m_cryptoRep->error());
            return;
        }
    }

    void MarketDataManager::saveToCryptoRepository(const TradeList& tradePairs) {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        QMutexLocker locker(&m_mutex);
        if (!m_cryptoRep->insertTrades(tradePairs)) {
            emit errorOccurred(m_cryptoRep->error());
            return;
        }
    }

    void MarketDataManager::saveToCandleRepository(const ItemCandle& newCandle) {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        QMutexLocker locker(&m_mutex);
        if (!m_candleRep->insertCandle(newCandle))
            emit errorOccurred(m_candleRep->error());
    }

    void MarketDataManager::saveToCandlesRepository(const CandleList& newCandles) {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        QMutexLocker locker(&m_mutex);
        if (!m_candleRep->insertCandles(newCandles))
            emit errorOccurred(m_candleRep->error());
    }

    void MarketDataManager::requestTradePairs() {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        if (!m_currentApi)
            return;

        emit messageSent("Loading spot pairs...");

        connect(m_currentApi, &Tools::BybitRestAPI::dataReceived, this, [this](const QJsonObject& data) {
            TradeList pairs;
            processRequestTradePairs(pairs, data);
            clearCryptoRepository();
            saveToCryptoRepository(pairs);
            emit tradePairsReceived(pairs);
            emit messageSent("Spot pairs loaded");
        }, Qt::SingleShotConnection);

        connect(m_currentApi, &Tools::BybitRestAPI::errorOccurred, this, &MarketDataManager::errorOccurred, Qt::SingleShotConnection);

        QUrlQuery params;
        params.addQueryItem("category", "spot");
        m_currentApi->requestEndpoint("/v5/market/instruments-info", params, LOADING_TIMEOUT);
    }

    void MarketDataManager::requestCandles(const QString& symbol, const QString& interval, qint64 start, qint64 end) {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        if (!m_currentApi)
            return;

        emit messageSent("Loading candles...");

        /*qInfo() << "Start:" << QDateTime::fromMSecsSinceEpoch(start).toString("dd.MM.yyyy HH:mm") << start
                << "End:" << QDateTime::fromMSecsSinceEpoch(end).toString("dd.MM.yyyy HH:mm") << end;*/

        connect(m_currentApi, &Tools::BybitRestAPI::dataReceived, this, [this, interval](const QJsonObject& data) {
            /*QJsonDocument doc(data);
            qInfo().noquote() << doc.toJson(QJsonDocument::Indented);*/
            CandleList candles;
            processRequestCandles(candles, interval, data);
            //qInfo() << "Candles received:" << candles.size();
            saveToCandlesRepository(candles);
            emit candlesReceived(candles);
            emit messageSent("Candles loaded");
        }, Qt::SingleShotConnection);

        connect(m_currentApi, &Tools::BybitRestAPI::errorOccurred, this, &MarketDataManager::errorOccurred, Qt::SingleShotConnection);

        QUrlQuery params;
        params.addQueryItem("category", "spot");
        params.addQueryItem("symbol", symbol);
        params.addQueryItem("interval", interval);
        params.addQueryItem("start", QString::number(start));
        params.addQueryItem("end", QString::number(end));
        params.addQueryItem("limit", "1000");
        m_currentApi->requestEndpoint("/v5/market/kline", params, LOADING_TIMEOUT);
    }

    void MarketDataManager::requestInfoAboutAccount() {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        if (!m_currentApi)
            return;

        emit messageSent("Getting account information...");

        connect(m_currentApi, &Tools::BybitRestAPI::dataReceived, this, [this](const QJsonObject& data) {
            emit infoAboutAccountReceived(data["retMsg"] == "OK");
            emit messageSent("Account information received");
        }, Qt::SingleShotConnection);

        connect(m_currentApi, &Tools::BybitRestAPI::errorOccurred, this, &MarketDataManager::errorOccurred, Qt::SingleShotConnection);

        m_currentApi->requestEndpoint("/v5/account/info", QUrlQuery(), LOADING_TIMEOUT);
    }

    void MarketDataManager::processRequestCandles(CandleList& candles, const QString& interval, const QJsonObject& data) {
        QJsonObject result = data["result"].toObject();
        QString symbol = result["symbol"].toString();
        QJsonArray list = result["list"].toArray();

        QMutexLocker locker(&m_mutex);
        for (const auto& obj : list) {
            QJsonArray itemArr = obj.toArray();
            ItemCandle candle;
            candle.m_symbol = symbol;
            candle.m_start = itemArr[0].toString().toDouble();
            candle.m_end = candle.m_start + 60000;
            candle.m_open = itemArr[1].toString().toDouble();
            candle.m_high = itemArr[2].toString().toDouble();
            candle.m_low = itemArr[3].toString().toDouble();
            candle.m_close = itemArr[4].toString().toDouble();
            candle.m_volume = itemArr[5].toString().toDouble();
            candle.m_turnover = itemArr[6].toString().toDouble();
            candle.m_confirm = true;
            candle.m_interval = interval;
            candles.append(candle);
        }
    }

    void MarketDataManager::processRequestTradePairs(TradeList& pairs, const QJsonObject& data) {
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

    void MarketDataManager::setAPI(const Tools::API& api) {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        if (!m_currentApi)
            return;

        m_currentApi->initAPI(api);
    }

} // namespace Engine