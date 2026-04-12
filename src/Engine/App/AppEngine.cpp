#include "Engine/App/AppEngine.hpp"

namespace Engine {

    AppEngine::AppEngine(QObject* parent) : QObject(parent) {
        m_webSocket = std::make_unique<BybitWebSocket>(this);

        // Подключаем сигналы
        connect(m_webSocket.get(), &BybitWebSocket::connected, this, &AppEngine::started);
        connect(m_webSocket.get(), &BybitWebSocket::disconnected, this, &AppEngine::stopped);
        connect(m_webSocket.get(), &BybitWebSocket::errorOccurred, this, &AppEngine::errorOccurred);

        qDebug() << "AppEngine created in thread:" << QThread::currentThread();

    }

    AppEngine::~AppEngine() {
        qDebug() << "AppEngine destructor in thread:" << QThread::currentThread();

        if (m_webSocket && m_webSocket->isOpen())
            m_webSocket->close();

        qDebug() << "AppEngine destructor end";
    }

    bool AppEngine::hasRunned() {
        return m_webSocket && m_webSocket->isOpen();
    }

    void AppEngine::run(const QUrl& baseEndpoint) {
        if (!m_webSocket)
            return;

        qDebug() << "AppEngine::run in thread:" << QThread::currentThread() << "Socket thread:" << m_webSocket->thread();

        m_webSocket->open(baseEndpoint);
    }

    void AppEngine::stop() {
        if (!m_webSocket)
            return;

        qDebug() << "AppEngine::stop in thread:" << QThread::currentThread();

        if (m_webSocket->isOpen()) {
            m_webSocket->disconnectFromStream();
            m_webSocket->close();
        }
    }

}