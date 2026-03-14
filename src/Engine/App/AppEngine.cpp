#include "Engine/App/AppEngine.hpp"

namespace Engine {

    AppEngine::AppEngine(QObject* parent) : QObject(parent) {
        m_webSocket = new BybitWebSocket(this);
    }

    void AppEngine::run(const QString& coin, QList<BybitWebSocket::Stream> streams) {

    }

    void AppEngine::stop() {

    }

}