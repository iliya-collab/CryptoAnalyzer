#include "Engine/App/AppWorker.hpp"

#include <QVariant>
#include <QList>
#include <QVariantList>

AppWorker::AppWorker(QObject* parent) : QObject(parent) {
    qDebug() << "--------------------------------------------------";

    qDebug() << "AppWorker created in thread:" << QThread::currentThread();

    m_loader = std::make_unique<Engine::AppEngineLoader>();
    m_engine = std::make_unique<Engine::AppEngine>();

    m_workerThread = new QThread(this);

    m_loader->moveToThread(m_workerThread);
    m_engine->moveToThread(m_workerThread);

    qDebug() << "AppEngineLoader moved to thread:" << m_loader->thread();
    qDebug() << "AppEngine moved to thread:" << m_engine->thread();

    connect(m_workerThread, &QThread::started, this, [this]() {
        QMetaObject::invokeMethod(m_loader.get(), [this] () {
            qDebug().noquote() << "AppEngineLoader::startLoading in thread:" << QThread::currentThread();
            m_loader->startLoading();
        }, Qt::QueuedConnection);
    });

    setupConnections();

    m_workerThread->start();

    qDebug() << "--------------------------------------------------";
}

AppWorker::~AppWorker() {
    qDebug() << "AppWorker destructor start in thread:" << QThread::currentThread();

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

    qDebug() << "AppWorker destructor end";
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
        m_hasStarted = true;
        qDebug().noquote() << "Engine started in thread:" << QThread::currentThread();

        auto lstTrades = m_loader->getData(Engine::tradeToString(m_trade));
        m_lstTrades.clear();
        for (const auto& iTrade : lstTrades)
            m_lstTrades.append(iTrade.symbol);
        emit tradeListChanged();

        emit engineStarted();
    });

    // Движок остановлен
    connect(m_engine.get(), &Engine::AppEngine::stopped, this, [this]() {
        m_hasStarted = false;
        qDebug().noquote() << "Engine stopped in thread:" << QThread::currentThread();

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
}

void AppWorker::setupConnections() {
    setupLoaderConnections();
    setupEngineConnections();
}

void AppWorker::startTrade() {
    if (!m_hasLoaded) {
        qDebug() << "Cannot start trade: not loaded";
        return;
    }

    QUrl url = Engine::tradeToBaseEndpoint(m_trade);
    QMetaObject::invokeMethod(m_engine.get(), [this, url]() {
        qDebug() << "AppWorker::startTrade in thread:" << QThread::currentThread();

        if (!m_engine->hasRunned()) {
            m_engine->run(url);
        } else {
            m_pendingUrl = url;
            m_engine->stop();
        }
    }, Qt::QueuedConnection);
}

QVariantList AppWorker::tradeList() const {
    return m_lstTrades;
}