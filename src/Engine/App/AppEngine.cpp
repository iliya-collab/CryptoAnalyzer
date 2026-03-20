#include "Engine/App/AppEngine.hpp"

namespace Engine {

    AppEngine::AppEngine(QObject* parent) : QObject(parent) {
        m_webSocket = new BybitWebSocket(this);

        connect(m_webSocket, &BybitWebSocket::connected, this, &AppEngine::onConnected);
        connect(m_webSocket, &BybitWebSocket::disconnected, this, &AppEngine::onDisconnected);
        connect(m_webSocket, &BybitWebSocket::errorOccurred, this, &AppEngine::onError);
    }

    void AppEngine::run(const QUrl& baseEndpont) {
        if (!m_webSocket->isOpen())
            m_webSocket->open(baseEndpont);
    }

    void AppEngine::stop() {
        m_webSocket->disconnectFromStream();
        m_webSocket->close();
    }

    void AppEngine::onConnected() {

    }

    void AppEngine::onDisconnected() {

    }

    void AppEngine::onError(const QString& error) {

    }

}