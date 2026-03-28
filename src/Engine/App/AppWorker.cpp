#include "Engine/App/AppWorker.hpp"

AppWorker::AppWorker(QObject* parent) : QObject(parent) {
    m_thread = new QThread(this);
    m_loader = new Engine::AppEngineLoader;
    m_engine = new Engine::AppEngine;

    m_loader->moveToThread(m_thread);
    m_engine->moveToThread(m_thread);

    connect(m_thread, &QThread::finished, m_loader, &QObject::deleteLater);
    connect(m_thread, &QThread::finished, m_engine, &QObject::deleteLater);
    connect(m_thread, &QThread::finished, m_thread, &QObject::deleteLater);

    setupConnections();

    if (!m_thread->isRunning())
        m_thread->start();
    QMetaObject::invokeMethod(m_loader, &Engine::AppEngineLoader::startLoading, Qt::QueuedConnection);
}

AppWorker::~AppWorker() {
    if (m_hasStarted) {
        QEventLoop loop;
        connect(m_engine, &Engine::AppEngine::stopped, &loop, &QEventLoop::quit);
        QMetaObject::invokeMethod(m_engine, &Engine::AppEngine::stop, Qt::QueuedConnection);
        loop.exec();
        m_hasStarted = false;
    }

    m_thread->quit();
    m_thread->wait();
}

void AppWorker::setupConnections() {
    // Завершение загрузки
    connect(m_loader, &Engine::AppEngineLoader::finished, this, [this](bool success) {
        qDebug().noquote() << "Loading engine finished";
        m_hasLoaded = success;
        emit loadingFinished(success);
    });

    // Прогресс загрузки
    connect(m_loader, &Engine::AppEngineLoader::progressChanged, [this](const QString& step, int current, int total) {
        double percent = (double)current/total*100;
    qDebug().noquote() << QString("Loaded %1% - %2").arg(percent, 6, 'f', 1).arg(step);
        emit loadingProgress(current, total);
    });

    // Ошибки загрузки
    connect(m_loader, &Engine::AppEngineLoader::errorOccurred, [this](const QString& error) {
        qDebug().noquote() << error;
        emit errorOccurred(error);
    });


    // Дижок запущен
    connect(m_engine, &Engine::AppEngine::started, [this]() {
        qDebug().noquote() << "Engine started";
    });

    // Дижок остановлен
    connect(m_engine, &Engine::AppEngine::stopped, [this]() {
        qDebug().noquote() << "Engine stopped";
    });

    // Ошибки движка
    connect(m_engine, &Engine::AppEngine::errorOccurred, [this](const QString& error) {
        qDebug().noquote() << "Error engine:" << error;
        emit errorOccurred(error);
    });
}

void AppWorker::startWork(Engine::TypesTrade trade) {
    QMetaObject::invokeMethod(m_engine, [this, trade]() {
        m_engine->run(Engine::tradeToBaseEndpoint(trade));
    }, Qt::QueuedConnection);
}