#include "AppWorker.hpp"

#include <QVariant>
#include <QList>
#include <QVariantList>

AppWorker::AppWorker(QObject* parent) : QObject(parent) {
    qDebug() << Q_FUNC_INFO << "created in:" << QThread::currentThread();
    m_loader = std::make_unique<Engine::AppEngineLoader>();
    m_engine = std::make_unique<Engine::AppEngine>();
    m_workerThread = new QThread(this);
}

AppWorker::~AppWorker() {
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

AppWorker* AppWorker::create(QQmlEngine *engine, QJSEngine *scriptEngine) {
    return new AppWorker();
}

void AppWorker::setupLoaderConnections() {
    // Ошибки загрузки
    connect(m_loader.get(), &Engine::AppEngineLoader::errorOccurred, this,  [this](const QString& error) {
        qDebug().noquote() << " * Loader error:" << error;
        emit errorOccurred(error);
    });

    connect(m_loader.get(), &Engine::AppEngineLoader::messageSent, this, &AppWorker::messageReceived);

    connect(m_loader.get(), &Engine::AppEngineLoader::downloadProgress, this, &AppWorker::downloadProgress);

}

void AppWorker::setupEngineConnections() {
    // Движок запущен
    connect(m_engine.get(), &Engine::AppEngine::started, this, [this]() {
        qDebug().noquote() << "------------------------------ Engine started ------------------------------";
        if (m_lastTrade.length() > 0)
            m_engine->addTrade(m_lastTrade);
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
        //qDebug().noquote() << "Kline received - latest update" << newKline.m_symbol << QTime::currentTime().toString();
        emit klineUpdated(newKline);
    });

}

void AppWorker::setupConnections() {
    setupLoaderConnections();
    setupEngineConnections();
}

// ----------------------------------------------------------------------------------------------------------------

void AppWorker::init() {
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

void AppWorker::run() {
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
    QMetaObject::invokeMethod(m_engine.get(), [this]() { m_engine->start(); }, Qt::QueuedConnection);
}

void AppWorker::restart() {
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
    QMetaObject::invokeMethod(m_engine.get(), [this]() { m_engine->stop(); }, Qt::QueuedConnection);
}

void AppWorker::interrupt() {
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
    QMetaObject::invokeMethod(m_engine.get(), [this]() { m_engine->stop(true); }, Qt::QueuedConnection);
}

void AppWorker::loadTradesFromRepository(const QString& category) {
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
    QMetaObject::invokeMethod(m_loader.get(), [this, category]() {
        auto lstTrades = (category == "ALL") ? m_loader->loadAllFromCryptoRepository() : m_loader->loadFromCryptoRepository(category);
        m_lstTrades.clear();
        for (const auto& iTrade : lstTrades)
            m_lstTrades.append(iTrade.symbol);
        emit tradeListChanged();
    }, Qt::QueuedConnection);
}

void AppWorker::loadTradesFromNetwork() {
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
    QMetaObject::invokeMethod(m_loader.get(), [this]() { m_loader->requestTradePairs(); }, Qt::QueuedConnection);
}

void AppWorker::loadCandlesFromNetwork(const QString& symbol, const QString& interval, int start, int end) {
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
    QMetaObject::invokeMethod(m_loader.get(), [this, symbol, interval, start, end]() { m_loader->requestCandles(symbol, interval, start, end); }, Qt::QueuedConnection);
}

void AppWorker::setAPI(const QString& apiKey, const QString& secretKey, bool isTestnet) {
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

    m_curAPI.m_apiKey = apiKey;
    m_curAPI.m_secretKey = secretKey;
    m_curAPI.m_isTestnet = isTestnet;

    m_engine->setAPI(m_curAPI);
    m_loader->setAPI(m_curAPI);
}

void AppWorker::checkAPI() {
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
    connect(m_loader.get(), &Engine::AppEngineLoader::infoAboutAccountReceived, this, &AppWorker::apiChecked, Qt::SingleShotConnection);
    QMetaObject::invokeMethod(m_engine.get(), [this]() { m_loader->requestInfoAboutAccount(); }, Qt::QueuedConnection);
}

void AppWorker::addTrade(const QString& pair) {
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
    m_lastTrade = pair;
    QMetaObject::invokeMethod(m_engine.get(), [this, pair]() { m_engine->addTrade(pair); }, Qt::QueuedConnection);
}

void AppWorker::saveCandle(const Engine::Kline& candle) {
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
    m_loader->saveToCandleRepository(candle);
}