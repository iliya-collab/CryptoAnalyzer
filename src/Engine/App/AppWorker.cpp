#include "Engine/App/AppWorker.hpp"

#include <QVariant>
#include <QList>
#include <QVariantList>

AppWorker::AppWorker(QObject* parent) : QObject(parent) {
    qDebug() << "--------------------------------------------------";

    qDebug() << Q_FUNC_INFO << "created in:" << QThread::currentThread();

    m_loader = std::make_unique<Engine::AppEngineLoader>();
    m_engine = std::make_unique<Engine::AppEngine>();

    m_workerThread = new QThread(this);

    m_loader->moveToThread(m_workerThread);
    m_engine->moveToThread(m_workerThread);

    qDebug() << "Loader moved to thread:" << m_loader->thread();
    qDebug() << "Engine moved to thread:" << m_engine->thread();

    connect(m_workerThread, &QThread::started, this, [this]() {
        QMetaObject::invokeMethod(m_loader.get(), [this] () {
            //qDebug().noquote() << "Loading has started in thread:" << QThread::currentThread();
            m_loader->startLoading();
        }, Qt::QueuedConnection);
    });

    setupConnections();

    m_workerThread->start();

    qDebug() << "--------------------------------------------------";
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
        qDebug().noquote() << "Loading engine finished in thread:" << QThread::currentThread();
        emit loadingFinished(success);
    });

    // Прогресс загрузки
    connect(m_loader.get(), &Engine::AppEngineLoader::progressChanged, this, [this](const QString& step, int current, int total) {
        double percent = (double)current/total*100;
        qDebug().noquote() << QString("Loaded %1% - %2").arg(percent, 6, 'f', 1).arg(step);
        emit loadingProgress(step, current, total);
    });

    // Ошибки загрузки
    connect(m_loader.get(), &Engine::AppEngineLoader::errorOccurred, this,  [this](const QString& error) {
        qDebug().noquote() << "Loader error:" << error;
        emit errorOccurred(error);
    });
}

void AppWorker::setupEngineConnections() {
    // Движок запущен
    connect(m_engine.get(), &Engine::AppEngine::started, this, [this]() {
        qDebug().noquote() << "Engine started";

        m_hasStarted = true;
        auto lstTrades = m_loader->getData(Engine::tradeToString(m_trade));
        m_lstTrades.clear();
        for (const auto& iTrade : lstTrades)
            m_lstTrades.append(iTrade.symbol);
        emit tradeListChanged();

        emit engineStarted();
    });

    // Движок остановлен
    connect(m_engine.get(), &Engine::AppEngine::stopped, this, [this]() {
        qDebug().noquote() << "Engine stopped";

        m_hasStarted = false;
        if (!m_pendingUrl.isEmpty()) {
            QUrl url = m_pendingUrl;
            m_pendingUrl.clear();
            QMetaObject::invokeMethod(m_engine.get(), [this, url]() { m_engine->run(url); }, Qt::QueuedConnection);
        }
    });

    // Ошибки движка
    connect(m_engine.get(), &Engine::AppEngine::errorOccurred, this, [this](const QString& error) {
        qDebug() << "Engine error:" << error;
        emit errorOccurred(error);
    });

    connect(m_engine.get(), &Engine::AppEngine::tickerUpdated, this, &AppWorker::tickerUpdated);
}

void AppWorker::setupConnections() {
    setupLoaderConnections();
    setupEngineConnections();
}

void AppWorker::startEngine() {
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

    if (!m_hasLoaded) {
        qDebug() << "The engine failed to load";
        return;
    }

    QUrl url = Engine::tradeToBaseEndpoint(m_trade);
    QMetaObject::invokeMethod(m_engine.get(), [this, url]() {
        if (!m_engine->hasRunned()) {
            m_engine->run(url);
        } else {
            m_pendingUrl = url;
            m_engine->stop();
        }

    }, Qt::QueuedConnection);
}

void AppWorker::startTrade(const QString& pair) {
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

    QMetaObject::invokeMethod(m_engine.get(), [this, pair]() {
        if (!m_engine->hasRunned()) {
            qDebug() << "Unable to start trade";
            return;
        }

        m_engine->addTrade(pair);

    }, Qt::QueuedConnection);
}

QVariantList AppWorker::tradeList() const {
    return m_lstTrades;
}