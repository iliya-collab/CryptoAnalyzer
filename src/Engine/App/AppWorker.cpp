#include "Engine/App/AppWorker.hpp"

#include <QVariant>
#include <QList>
#include <QVariantList>

AppWorker::AppWorker(QObject* parent) : QObject(parent) {
    //qDebug() << "--------------------------------------------------";

    //qDebug() << Q_FUNC_INFO << "created in:" << QThread::currentThread();

    m_loader = std::make_unique<Engine::AppEngineLoader>();
    m_engine = std::make_unique<Engine::AppEngine>();
    m_config = std::make_unique<Engine::AppEngineСonfiguration>();

    m_workerThread = new QThread(this);

    m_loader->moveToThread(m_workerThread);
    m_engine->moveToThread(m_workerThread);
    m_config->moveToThread(m_workerThread);

    //qDebug() << "Loader moved to thread:" << m_loader->thread();
    //qDebug() << "Engine moved to thread:" << m_engine->thread();

    connect(m_workerThread, &QThread::started, this, [this]() {
        QMetaObject::invokeMethod(m_loader.get(), [this] () {
            m_config->loadConfig();

            auto config = m_config->getСonfiguration();
            m_curAPI = config.m_api.value(config.m_defaultKey);

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
    if (m_hasStarted) {
        QEventLoop loop;
        // Подключаемся к сигналу stopped
        connect(m_engine.get(), &Engine::AppEngine::stopped, &loop, &QEventLoop::quit, Qt::SingleShotConnection);
        // Останавливаем движок в его потоке
        QMetaObject::invokeMethod(m_engine.get(), &Engine::AppEngine::stop, Qt::QueuedConnection);
        loop.exec();
        m_hasStarted = false;
    }

    // Останавливаем поток
    m_workerThread->quit();
    m_workerThread->wait();

    qDebug() << Q_FUNC_INFO << "finished";
}

void AppWorker::setupLoaderConnections() {
    // Завершение загрузки
    connect(m_loader.get(), &Engine::AppEngineLoader::finished, this, [this](bool success) {
        m_hasLoaded = success;
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

        m_hasStarted = true;
        auto lstTrades = m_loader->getData(m_loadTrade);
        m_lstTrades.clear();
        for (const auto& iTrade : lstTrades)
            m_lstTrades.append(iTrade.symbol);
        emit tradeListChanged();

        emit engineStarted();
    });

    // Движок остановлен
    connect(m_engine.get(), &Engine::AppEngine::stopped, this, [this]() {
        qDebug().noquote() << "------------------------------ Engine stopped ------------------------------";

        m_hasStarted = false;
        QMetaObject::invokeMethod(m_engine.get(), [this]() { m_engine->run(m_pendingEndpoint); }, Qt::QueuedConnection);
    });

    // Ошибки движка
    connect(m_engine.get(), &Engine::AppEngine::errorOccurred, this, [this](const QString& error) {
        qDebug() << " * Engine error:" << error;
        emit errorOccurred(error);
    });

    connect(m_engine.get(), &Engine::AppEngine::tickerUpdated, this, &AppWorker::tickerUpdated);
}

void AppWorker::setupConnections() {
    setupLoaderConnections();
    setupEngineConnections();
}

void AppWorker::run(Engine::WebSocketEndpoints endpoint) {
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

    if (!m_hasLoaded) {
        qDebug() << "The engine failed to load";
        return;
    }

    switch (endpoint) {
    case Engine::WebSocketEndpoints::SPOT:
        m_loadTrade = "spot";
        break;
    case Engine::WebSocketEndpoints::LINEAR:
        m_loadTrade = "linear";
        break;
    case Engine::WebSocketEndpoints::INVERSE:
        m_loadTrade = "inverse";
        break;
    case Engine::WebSocketEndpoints::OPTION:
        m_loadTrade = "option";
        break;
    default:
        m_loadTrade = "";
        break;
    }

    m_pendingEndpoint = Engine::WebSocketEndpoints::NONE;
    QMetaObject::invokeMethod(m_engine.get(), [this, endpoint]() {
        if (!m_engine->hasRunned())
            m_engine->run(endpoint);
        else {
            m_pendingEndpoint = endpoint;
            m_engine->stop();
        }
    }, Qt::QueuedConnection);
}

void AppWorker::setAPI(const Engine::API& api) {
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

    m_curAPI = api;
    m_engine->setAPI(m_curAPI);
    m_loader->setAPI(m_curAPI);
}

void AppWorker::addTrade(const QString& pair) {
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

    QMetaObject::invokeMethod(m_engine.get(), [this, pair]() {
        if (!m_engine->hasRunned()) {
            qDebug() << "Unable to start trade";
            return;
        }
        m_engine->addTrade(pair);
    }, Qt::QueuedConnection);
}

void AppWorker::filter(const QString& pair, bool on) {
    m_engine->enableFilter(pair, on);
}

void AppWorker::saveAPI(const QString& name, const QString& apiKey, const QString& secretKey, bool isTestnet) {
    Engine::API api = { apiKey, secretKey, isTestnet };
    m_config->saveARIKey(name, api);
}

QVariantList AppWorker::getTradeList() const {
    return m_lstTrades;
}

QVariantList AppWorker::getAPIKeys() const {
    return QVariantList();
}