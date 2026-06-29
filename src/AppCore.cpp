#include "AppCore.hpp"
#include <QVariant>
#include <QList>
#include <QVariantList>

AppCore::AppCore(QObject* parent) : QObject(parent) {
    qDebug() << Q_FUNC_INFO << "created in:" << QThread::currentThread();
    m_loader = std::make_unique<Engine::AppEngineLoader>();
    m_engine = std::make_unique<Engine::AppEngine>();
    m_workerThread = new QThread(this);
}

AppCore::~AppCore() {
    qDebug() << Q_FUNC_INFO << "launched from:" << QThread::currentThread();

    // Останавливаем движок перед удалением
    if (m_engine->hasRunned()) {
        QEventLoop loop;
        // Подключаемся к сигналу stopped
        connect(m_engine.get(), &Engine::AppEngine::stopped, &loop, &QEventLoop::quit, Qt::SingleShotConnection);
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
    connect(m_loader.get(), &Engine::AppEngineLoader::errorOccurred, this,  [this](const QString& error) {
        qDebug().noquote() << " * Loader error:" << error;
        emit errorOccurred(error);
    });

    connect(m_loader.get(), &Engine::AppEngineLoader::messageSent, this, &AppCore::messageReceived);

    connect(m_loader.get(), &Engine::AppEngineLoader::downloadProgress, this, &AppCore::downloadProgress);

}

void AppCore::setupEngineConnections() {
    // Движок запущен
    connect(m_engine.get(), &Engine::AppEngine::started, this, [this]() {
        qDebug().noquote() << "------------------------------ Engine started ------------------------------";
        if (m_lastTrade.length() > 0)
            m_engine->addTrade(m_lastTrade);
        m_startTime = m_engine->getStartTime();
        qDebug() << "Start time:" << QDateTime::fromMSecsSinceEpoch(m_startTime).toString("hh:mm:ss");
        emit startTimeChanged(m_startTime);
        emit engineStarted();
    });

    // Движок остановлен
    connect(m_engine.get(), &Engine::AppEngine::stopped, this, [this]() {
        qDebug().noquote() << "------------------------------ Engine stopped ------------------------------";
        emit engineStopped();
    });

    // Ошибки движка
    connect(m_engine.get(), &Engine::AppEngine::errorOccurred, this, [this](const QString& error) {
        qDebug() << " * Engine error:" << error;
        emit errorOccurred(error);
    });

    connect(m_engine.get(), &Engine::AppEngine::tickerUpdated, this, [this](const Engine::Ticker& newTicker) {
        //qDebug().noquote() << "Ticker received - latest update" << newTicker.m_symbol << QTime::currentTime().toString();
        emit tickerUpdated(newTicker);
    });
    connect(m_engine.get(), &Engine::AppEngine::orderBookUpdated, this, [this](const Engine::Orderbook& newOrderbook) {
        //qDebug().noquote() << "Orderbook received - latest update" << newOrderBook.m_symbol << QTime::currentTime().toString();
        emit orderbookUpdated(newOrderbook);
    });
    connect(m_engine.get(), &Engine::AppEngine::klineUpdated, this, [this](const Engine::Kline& newKline) {
        //qDebug().noquote() << "Kline received - latest update" << newKline.m_symbol << newKline.m_timestamp << QTime::currentTime().toString();
        saveCandle(newKline);
        emit klineUpdated(newKline);
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

    m_loader->moveToThread(m_workerThread);
    m_engine->moveToThread(m_workerThread);

    m_loader->init();

    m_engine->setAPI(m_curAPI);
    m_loader->setAPI(m_curAPI);

    setupConnections();

    m_workerThread->start();
}

void AppCore::run() {
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
    QMetaObject::invokeMethod(m_engine.get(), [this]() { m_engine->start(); }, Qt::QueuedConnection);
}

void AppCore::restart() {
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
    QMetaObject::invokeMethod(m_engine.get(), [this]() { m_engine->stop(); }, Qt::QueuedConnection);
}

void AppCore::interrupt() {
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
    QMetaObject::invokeMethod(m_engine.get(), [this]() { m_engine->stop(true); }, Qt::QueuedConnection);
}

void AppCore::loadTradesFromRepository(const QString& category) {
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

    auto newTradeList = (category == "ALL") ? m_loader->loadAllFromCryptoRepository() :
                            m_loader->loadFromCryptoRepository(category);

    m_tradeList.clear();

    for (const auto& iTrade : newTradeList)
        m_tradeList.append(iTrade.symbol);

    emit tradeListChanged();

    /*QMetaObject::invokeMethod(m_loader.get(), [this, category]() {
        auto newTradeList = (category == "ALL") ? m_loader->loadAllFromCryptoRepository() :
                                                m_loader->loadFromCryptoRepository(category);

        m_tradeList.clear();

        for (const auto& iTrade : newTradeList)
            m_tradeList.append(iTrade.symbol);

        emit tradeListChanged();
    }, Qt::QueuedConnection);*/
}

void AppCore::loadTradesFromNetwork() {
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

    connect(m_loader.get(), &Engine::AppEngineLoader::tradePairsReceived, this, [this](const Engine::TradeList& newTradeList) {
        m_tradeList.clear();
        for (const auto& iTrade : newTradeList)
            m_tradeList.append(iTrade.symbol);
        emit tradeListChanged();
    }, Qt::SingleShotConnection);

    QMetaObject::invokeMethod(m_loader.get(), [this]() { m_loader->requestTradePairs(); }, Qt::QueuedConnection);
}

void AppCore::loadCandlesFromNetwork(const QString& symbol, const QString& interval, int start, int end) {
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

    connect(m_loader.get(), &Engine::AppEngineLoader::candlesReceived, this, [this](const Engine::CandleList& newCandleList) {
        for (const auto& iCandle : newCandleList) {
            QVariantMap map;
            map["timestamp"] = iCandle.m_timestamp;
            map["open"] = iCandle.m_open;
            map["close"] = iCandle.m_close;
            map["high"] = iCandle.m_high;
            map["low"] = iCandle.m_low;
            map["isOpen"] = iCandle.m_confirm;
            m_loadedCandles.insert(0, map);
        }
        emit loadedCandlesChanged();
    }, Qt::SingleShotConnection);

    QMetaObject::invokeMethod(m_loader.get(), [this, symbol, interval, start, end]() { m_loader->requestCandles(symbol, interval, start, end); }, Qt::QueuedConnection);
}

void AppCore::setAPI(const QString& apiKey, const QString& secretKey, bool isTestnet) {
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

    m_curAPI.m_apiKey = apiKey;
    m_curAPI.m_secretKey = secretKey;
    m_curAPI.m_isTestnet = isTestnet;

    m_engine->setAPI(m_curAPI);
    m_loader->setAPI(m_curAPI);
}

void AppCore::checkAPI() {
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
    connect(m_loader.get(), &Engine::AppEngineLoader::infoAboutAccountReceived, this, &AppCore::apiChecked, Qt::SingleShotConnection);
    QMetaObject::invokeMethod(m_engine.get(), [this]() { m_loader->requestInfoAboutAccount(); }, Qt::QueuedConnection);
}

void AppCore::addTrade(const QString& pair) {
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
    m_lastTrade = pair;
    QMetaObject::invokeMethod(m_engine.get(), [this, pair]() { m_engine->addTrade(pair); }, Qt::QueuedConnection);
}

void AppCore::saveCandle(const Engine::ItemCandle& candle) {
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

    QVariantMap map;
    map["timestamp"] = candle.m_timestamp;
    map["open"] = candle.m_open;
    map["close"] = candle.m_close;
    map["high"] = candle.m_high;
    map["low"] = candle.m_low;
    map["isOpen"] = candle.m_confirm;
    m_loadedCandles.append(map);

    m_loader->saveToCandleRepository(candle);

    emit loadedCandlesChanged();
}