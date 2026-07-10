#include "AppCore.hpp"
#include "Core/Tools/LogManager.hpp"
#include <QVariant>
#include <QList>
#include <QVariantList>

AppCore::AppCore(QObject* parent) : QObject(parent) {

    LogManager::instance().setDebugEnabled(false);

    qDebug() << Q_FUNC_INFO << "created in:" << QThread::currentThread();

    m_manager = std::make_unique<MarketDataManager>();
    m_streamer = std::make_unique<MarketDataStreamer>();
    m_workerThread = new QThread(this);
}

AppCore::~AppCore() {
    qDebug() << Q_FUNC_INFO << "launched from:" << QThread::currentThread();

    // Останавливаем движок перед удалением
    if (m_streamer->hasRunned()) {
        QEventLoop loop;
        // Подключаемся к сигналу stopped
        connect(m_streamer.get(), &MarketDataStreamer::stopped, &loop, &QEventLoop::quit, Qt::SingleShotConnection);
        // Останавливаем движок в его потоке
        interrupt();
        loop.exec();
    }

    // Останавливаем поток
    m_workerThread->quit();
    m_workerThread->wait();
}

AppCore* AppCore::create(QQmlEngine *engine, QJSEngine *scriptEngine) {
    return new AppCore();
}


void AppCore::setupLoaderConnections() {
    // Ошибки загрузки
    connect(m_manager.get(), &MarketDataManager::errorOccurred, this,  [this](const QString& error) {
        qDebug().noquote() << " * Loader error:" << error;
        emit errorOccurred(error);
    });

    connect(m_manager.get(), &MarketDataManager::messageSent, this, &AppCore::messageReceived);

    connect(m_manager.get(), &MarketDataManager::downloadProgress, this, &AppCore::downloadProgress);

}

void AppCore::setupEngineConnections() {
    // Движок запущен
    connect(m_streamer.get(), &MarketDataStreamer::started, this, [this]() {
        qDebug().noquote() << "------------------------------ Engine started ------------------------------";
        if (m_lastTrade.length() > 0)
            m_streamer->addTrade(m_lastTrade);
        m_startTime = m_streamer->getStartTime();
        qDebug() << "Start time:" << QDateTime::fromMSecsSinceEpoch(m_startTime).toString("hh:mm:ss");
        emit startTimeChanged(m_startTime);
        emit engineStarted();
    });

    // Движок остановлен
    connect(m_streamer.get(), &MarketDataStreamer::stopped, this, [this]() {
        qDebug().noquote() << "------------------------------ Engine stopped ------------------------------";
        emit engineStopped();
    });

    // Ошибки движка
    connect(m_streamer.get(), &MarketDataStreamer::errorOccurred, this, [this](const QString& error) {
        qDebug() << " * Engine error:" << error;
        emit errorOccurred(error);
    });

    connect(m_streamer.get(), &MarketDataStreamer::tickerUpdated, this, [this](const Ticker& newTicker) {
        //qDebug().noquote() << "Ticker received - latest update" << newTicker.m_symbol << QTime::currentTime().toString();
        emit tickerUpdated(newTicker);
    });
    connect(m_streamer.get(), &MarketDataStreamer::orderBookUpdated, this, [this](const Orderbook& newOrderbook) {
        //qDebug().noquote() << "Orderbook received - latest update" << newOrderBook.m_symbol << QTime::currentTime().toString();
        emit orderbookUpdated(newOrderbook);
    });
    connect(m_streamer.get(), &MarketDataStreamer::klineUpdated, this, [this](const Kline& newKline) {
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

}

void AppCore::setupConnections() {
    setupLoaderConnections();
    setupEngineConnections();
}

// ----------------------------------------------------------------------------------------------------------------

void AppCore::init() {
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

    if (wasInit.load()) {
        emit errorOccurred("Reinitialization engine");
        return;
    }

    wasInit.store(true);

    m_manager->moveToThread(m_workerThread);
    m_streamer->moveToThread(m_workerThread);

    m_manager->init();

    m_streamer->setAPI(m_curAPI);
    m_manager->setAPI(m_curAPI);

    setupConnections();

    m_workerThread->start();
}

void AppCore::run() {
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
    QMetaObject::invokeMethod(m_streamer.get(), [this]() { m_streamer->start(); }, Qt::QueuedConnection);
}

void AppCore::restart() {
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
    QMetaObject::invokeMethod(m_streamer.get(), [this]() { m_streamer->stop(); }, Qt::QueuedConnection);
}

void AppCore::interrupt() {
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
    QMetaObject::invokeMethod(m_streamer.get(), [this]() { m_streamer->stop(true); }, Qt::QueuedConnection);
}

void AppCore::loadTradesFromRepository(const QString& category) {
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

    auto newTradeList = (category == "ALL") ? m_manager->loadAllFromCryptoRepository() :
                            m_manager->loadFromCryptoRepository(category);

    m_tradeList.clear();

    for (const auto& iTrade : newTradeList)
        m_tradeList.append(iTrade.symbol);

    emit tradeListChanged();
}

void AppCore::loadTradesFromNetwork() {
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

    connect(m_manager.get(), &MarketDataManager::tradePairsReceived, this, [this](const TradeList& newTradeList) {
        m_tradeList.clear();
        for (const auto& iTrade : newTradeList)
            m_tradeList.append(iTrade.symbol);
        emit tradeListChanged();
    }, Qt::SingleShotConnection);

    QMetaObject::invokeMethod(m_manager.get(), [this]() { m_manager->requestTradePairs(); }, Qt::QueuedConnection);
}

void AppCore::loadCandlesFromNetwork(const QString& symbol, const QString& interval, qint64 start, qint64 end) {
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

    connect(m_manager.get(), &MarketDataManager::candlesReceived, this, [this](const CandleList& newCandleList) {
        for (const auto& iCandle : newCandleList) {
            QVariantMap map;
            map["start"] = iCandle.m_start;
            map["end"] = iCandle.m_end;
            map["open"] = iCandle.m_open;
            map["close"] = iCandle.m_close;
            map["high"] = iCandle.m_high;
            map["low"] = iCandle.m_low;
            map["isConfirm"] = iCandle.m_confirm;
            m_candleSeries.insert(0, map);
        }
        //qInfo() << "Updated candle series:" << m_candleSeries.size();
        emit candleSeriesChanged();
    }, Qt::SingleShotConnection);

    QMetaObject::invokeMethod(m_manager.get(), [this, symbol, interval, start, end]() { m_manager->requestCandles(symbol, interval, start, end); }, Qt::QueuedConnection);
}

void AppCore::setAPI(const QString& apiKey, const QString& secretKey, bool isTestnet) {
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

    m_curAPI.m_apiKey = apiKey;
    m_curAPI.m_secretKey = secretKey;
    m_curAPI.m_isTestnet = isTestnet;

    m_streamer->setAPI(m_curAPI);
    m_manager->setAPI(m_curAPI);
}

void AppCore::checkAPI() {
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
    connect(m_manager.get(), &MarketDataManager::infoAboutAccountReceived, this, &AppCore::apiChecked, Qt::SingleShotConnection);
    QMetaObject::invokeMethod(m_streamer.get(), [this]() { m_manager->requestInfoAboutAccount(); }, Qt::QueuedConnection);
}

void AppCore::addTrade(const QString& pair) {
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
    m_lastTrade = pair;
    QMetaObject::invokeMethod(m_streamer.get(), [this, pair]() { m_streamer->addTrade(pair); }, Qt::QueuedConnection);
}

void AppCore::addCandle(const ItemCandle& candle) {
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

    QVariantMap map;
    map["start"] = candle.m_start;
    map["end"] = candle.m_end;
    map["open"] = candle.m_open;
    map["close"] = candle.m_close;
    map["high"] = candle.m_high;
    map["low"] = candle.m_low;
    map["isConfirm"] = candle.m_confirm;

    m_candleSeries.append(map);
    emit candleSeriesChanged();
}

void AppCore::updateCandle(const ItemCandle& candle) {
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

    if (!m_candleSeries.isEmpty()) {
        QVariantMap map = m_candleSeries.last().toMap();

        map["close"] = candle.m_close;
        map["high"] = candle.m_high;
        map["low"] = candle.m_low;

        m_candleSeries.replace(m_candleSeries.count() - 1, map);
        emit candleSeriesChanged();
    }
}