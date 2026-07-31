#include "AppCore.hpp"
#include "Tools/LogManager.hpp"
#include <QVariant>
#include <QList>
#include <QVariantList>

AppCore::AppCore(QObject* parent) : QObject(parent) {

    Core::Tools::LogManager::instance().setDebugEnabled(false);

    m_manager = std::make_unique<Core::MarketDataManager>();
    m_streamer = std::make_unique<Core::MarketDataStreamer>();
    m_asks = std::make_unique<Core::Tools::OrderbookSideModel>(Core::Tools::OrderbookSideModel::Side::Ask);
    m_bids = std::make_unique<Core::Tools::OrderbookSideModel>(Core::Tools::OrderbookSideModel::Side::Bid);
    m_trades = std::make_unique<Core::Tools::TradeModel>();
}

AppCore::~AppCore() {
    if (m_streamer->hasRunned()) {
        QEventLoop loop;
        connect(m_streamer.get(), &Core::MarketDataStreamer::stopped, &loop, &QEventLoop::quit, Qt::SingleShotConnection);
        interrupt();
        loop.exec();
    }
}

void AppCore::setupManagerConnections() {
    connect(m_manager.get(), &Core::MarketDataManager::errorOccurred, this,  [this](const QString& error) {
        qCritical().noquote() << "Error:" << error;
        emit errorOccurred(error);
    });

    connect(m_manager.get(), &Core::MarketDataManager::messageSent, this, &AppCore::messageReceived);

    connect(m_manager.get(), &Core::MarketDataManager::downloadProgress, this, &AppCore::downloadProgress);

}

void AppCore::setupStreamerConnections() {
    connect(m_streamer.get(), &Core::MarketDataStreamer::started, this, [this]() {
        qInfo().noquote() << "App core started";
        if (m_lastTrade.length() > 0)
            m_streamer->addTrade(m_lastTrade);
        emit started();
    });

    connect(m_streamer.get(), &Core::MarketDataStreamer::stopped, this, [this]() {
        qInfo().noquote() << "App core stopped";
        emit stopped();
    });

    connect(m_streamer.get(), &Core::MarketDataStreamer::errorOccurred, this, [this](const QString& error) {
        qCritical() << "Error:" << error;
        emit errorOccurred(error);
    });

    connect(m_streamer.get(), &Core::MarketDataStreamer::pingMeasured, this, [this](qint64 pingMs) {
        m_pingMs = pingMs;
        emit pingMsChanged();
    });

    connect(m_streamer.get(), &Core::MarketDataStreamer::tickerUpdated, this, [this](const Core::Tools::Ticker& newTicker) {
        //qDebug().noquote() << "Ticker received - latest update" << newTicker.m_symbol << QTime::currentTime().toString();
        m_ticker = newTicker;
        emit tickerChanged();
    });

    connect(m_streamer.get(), &Core::MarketDataStreamer::orderBookUpdated, this, [this](const Core::Tools::Orderbook& newOrderbook) {
        //qDebug().noquote() << "Orderbook received - latest update" << newOrderBook.m_symbol << QTime::currentTime().toString();
        m_asks->update(newOrderbook.m_asks);
        m_bids->update(newOrderbook.m_bids);
        emit asksChanged();
        emit bidsChanged();
    });

    connect(m_streamer.get(), &Core::MarketDataStreamer::klineUpdated, this, [this](const Core::Tools::Kline& newKline) {
        //qDebug().noquote() << "Kline received - latest update" << newKline.m_confirm;
        static bool waitNewCandle = true;

        if (waitNewCandle) {
            addCandle(newKline);
            waitNewCandle = false;
            emit candleSeriesChanged();
            return;
        }

        updateCandle(newKline);

        if (newKline.m_confirm) {
            waitNewCandle = true;
            auto& lastCandle = m_candleSeries.last();
            auto map = lastCandle.toMap();
            map["isConfirm"] = true;
            lastCandle = map;
            m_manager->saveToCandleRepository(newKline);
        }
    });

    connect(m_streamer.get(), &Core::MarketDataStreamer::publicTradeUpdated, this, [this](const Core::Tools::PublicTrades& newPublicTrades) {
        m_trades->addTradeBatch(newPublicTrades.m_items);
        emit tradesChanged();
    });

}

void AppCore::setupConnections() {
    setupManagerConnections();
    setupStreamerConnections();
}

// ----------------------------------------------------------------------------------------------------------------

void AppCore::init() {
    if (wasInit.load()) {
        emit errorOccurred("Reinitialization core");
        return;
    }

    wasInit.store(true);

    auto res = Core::ConfigurationManager::instance().read();
    if (!res.has_value()) {
        qCritical() << res.error();
        qWarning() << "The application will use default configuration";
    }

    auto config = res.value();
    m_api.m_apiKey = config.m_apiKey;
    m_api.m_secretKey = config.m_secretKey;
    m_api.m_isTestnet = config.m_isTestnet;

    m_streamer->setAPI(m_api);
    m_manager->init();
    m_manager->setAPI(m_api);

    setupConnections();

    if (config.m_autoConnection)
        m_streamer->start();
}

void AppCore::run() {
    m_streamer->start();
}

void AppCore::restart() {
    m_streamer->stop();
}

void AppCore::interrupt() {
    m_streamer->stop(true);
}

void AppCore::loadTradesFromRepository(const QString& category) {
    auto newTradeList = (category == "ALL") ? m_manager->loadAllFromCryptoRepository() :
                            m_manager->loadFromCryptoRepository(category);

    m_tradeList.clear();

    for (const auto& iTrade : newTradeList)
        m_tradeList.append(iTrade.symbol);

    emit tradeListChanged();
}

void AppCore::loadTradesFromNetwork() {
    connect(m_manager.get(), &Core::MarketDataManager::tradePairsReceived, this, [this](const Core::TradeList& newTradeList) {
        m_tradeList.clear();
        for (const auto& iTrade : newTradeList)
            m_tradeList.append(iTrade.symbol);
        emit tradeListChanged();
    }, Qt::SingleShotConnection);

    m_manager->requestTradePairs();
}

void AppCore::loadCandlesFromNetwork(const QString& symbol, const QString& interval, qint64 start, qint64 end) {
    connect(m_manager.get(), &Core::MarketDataManager::candlesReceived, this, [this, interval](const Core::CandleList& newCandleList) {
        for (const auto& iCandle : newCandleList) {
            QVariantMap map;
            map["start"] = iCandle.m_start;
            map["end"] = iCandle.m_end;
            map["open"] = iCandle.m_open;
            map["close"] = iCandle.m_close;
            map["high"] = iCandle.m_high;
            map["low"] = iCandle.m_low;
            map["volume"] = iCandle.m_volume;
            map["turnover"] = iCandle.m_turnover;
            map["isConfirm"] = iCandle.m_confirm;
            map["interval"] = interval;
            m_candleSeries.insert(0, map);
        }
        emit candleSeriesChanged();
    }, Qt::SingleShotConnection);

    m_manager->requestCandles(symbol, interval, start, end);
}

void AppCore::setAPI(const QString& apiKey, const QString& secretKey, bool isTestnet) {
    m_api.m_apiKey = apiKey;
    m_api.m_secretKey = secretKey;
    m_api.m_isTestnet = isTestnet;

    m_streamer->setAPI(m_api);
    m_manager->setAPI(m_api);
}

void AppCore::saveAPI(const QString& apiKey, const QString& secretKey, bool isTestnet) {
    auto& config = Core::ConfigurationManager::instance();
    config.setAPI(apiKey, secretKey, isTestnet);
    config.write();
}

void AppCore::requestAccount() {
    m_manager->requestInfoAboutAccount();
}

void AppCore::addTrade(const QString& pair) {
    m_lastTrade = pair;
    m_streamer->addTrade(pair);
}

void AppCore::addCandle(const Core::ItemCandle& candle) {
    QVariantMap map;
    map["start"] = candle.m_start;
    map["end"] = candle.m_end;
    map["open"] = candle.m_open;
    map["close"] = candle.m_close;
    map["high"] = candle.m_high;
    map["low"] = candle.m_low;
    map["volume"] = candle.m_volume;
    map["turnover"] = candle.m_turnover;
    map["isConfirm"] = candle.m_confirm;
    map["interval"] = candle.m_interval;

    m_candleSeries.append(map);
    emit candleSeriesChanged();
}

void AppCore::updateCandle(const Core::ItemCandle& candle) {
    if (!m_candleSeries.isEmpty()) {
        QVariantMap map = m_candleSeries.last().toMap();

        map["close"] = candle.m_close;
        map["high"] = candle.m_high;
        map["low"] = candle.m_low;
        map["volume"] = candle.m_volume;
        map["turnover"] = candle.m_turnover;

        m_candleSeries.replace(m_candleSeries.count() - 1, map);
        emit candleSeriesChanged();
    }
}