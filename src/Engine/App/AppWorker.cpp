#include "Engine/App/AppWorker.hpp"

#include <QVariant>
#include <QList>
#include <QVariantList>
#include <QRandomGenerator>

AppWorker::AppWorker(QObject* parent) : QObject(parent) {
    //qDebug() << "--------------------------------------------------";

    qDebug() << Q_FUNC_INFO << "created in:" << QThread::currentThread();

    m_loader = std::make_unique<Engine::AppEngineLoader>();
    m_engine = std::make_unique<Engine::AppEngine>();

    m_workerThread = new QThread(this);

    m_loader->moveToThread(m_workerThread);
    m_engine->moveToThread(m_workerThread);

    //qDebug() << "Loader moved to thread:" << m_loader->thread();
    //qDebug() << "Engine moved to thread:" << m_engine->thread();

    connect(m_workerThread, &QThread::started, this, [this]() {
        QMetaObject::invokeMethod(m_loader.get(), [this] () {
            m_engine->setAPI(m_curAPI);
            m_loader->setAPI(m_curAPI);
            m_loader->startLoading();
        }, Qt::QueuedConnection);
    });

    setupConnections();

    m_workerThread->start();

    //qDebug() << "--------------------------------------------------";
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

    qDebug() << Q_FUNC_INFO << "finished";
}

void AppWorker::setupLoaderConnections() {
    // Завершение загрузки
    connect(m_loader.get(), &Engine::AppEngineLoader::finished, this, [this](bool success) {
        qDebug().noquote() << "------------------------------ Loading engine finished ------------------------------";
        emit loadingFinished(success);
    });

    // Прогресс загрузки
    connect(m_loader.get(), &Engine::AppEngineLoader::progressChanged, this, [this](const QString& step, int current, int total) {
        double percent = (double)current/total*100;
        qDebug().noquote() << QString(" * Loaded %1% - %2").arg(percent, 6, 'f', 1).arg(step);
        emit loadingProgress(step, current, total);
    });

    // Ошибки загрузки
    connect(m_loader.get(), &Engine::AppEngineLoader::errorOccurred, this,  [this](const QString& error) {
        qDebug().noquote() << " * Loader error:" << error;
        emit errorOccurred(error);
    });
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

    connect(m_engine.get(), &Engine::AppEngine::tickerUpdated, this, [this](const Engine::stTicker& newTicker) {
        //qDebug().noquote() << "Ticker received - latest update" << newTicker.m_symbol << QTime::currentTime().toString();
        emit tickerUpdated(newTicker);
    });
    connect(m_engine.get(), &Engine::AppEngine::orderBookUpdated, this, [this](const Engine::stOrderBook& newOrderBook) {
        //qDebug().noquote() << "Orderbook received - latest update" << newOrderBook.m_symbol << QTime::currentTime().toString();
        emit orderBookUpdated(newOrderBook);
    });
    connect(m_engine.get(), &Engine::AppEngine::klineUpdated, this, [this](const Engine::stKline& newKline) {
        //qDebug().noquote() << "Kline received - latest update" << newKline.m_symbol << QTime::currentTime().toString();
        emit klineUpdated(newKline);
    });

}

void AppWorker::setupConnections() {
    setupLoaderConnections();
    setupEngineConnections();
}

// ----------------------------------------------------------------------------------------------------------------

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

void AppWorker::loadTradingPairs(const QString& category) {
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

    auto lstTrades = m_loader->loadTradingPairs(category);
    m_lstTrades.clear();
    for (const auto& iTrade : lstTrades)
        m_lstTrades.append(iTrade.symbol);

    emit tradeListChanged();
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

    connect(m_loader.get(), &Engine::AppEngineLoader::apiChecked, this, &AppWorker::apiChecked, Qt::SingleShotConnection);

    QMetaObject::invokeMethod(m_engine.get(), [this]() { m_loader->checkAPI(); }, Qt::QueuedConnection);
}

void AppWorker::addTrade(const QString& pair) {
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

    m_lastTrade = pair;
    QMetaObject::invokeMethod(m_engine.get(), [this, pair]() { m_engine->addTrade(pair); }, Qt::QueuedConnection);
}