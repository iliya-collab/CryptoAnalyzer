#include "Engine/App/AppEngine.hpp"

namespace Engine {

    AppEngine::AppEngine(QObject* parent) : QObject(parent) {
        m_webSocket = std::make_unique<BybitWebSocket>(this);

        // Обработка основных сигналов
        connect(m_webSocket.get(), &BybitWebSocket::connected, this, &AppEngine::started);
        connect(m_webSocket.get(), &BybitWebSocket::disconnected, this, &AppEngine::stopped);
        connect(m_webSocket.get(), &BybitWebSocket::errorOccurred, this, &AppEngine::errorOccurred);

        connect(m_webSocket.get(), &BybitWebSocket::updatedTicker, this, [](const stTicker& ticker) {
            qDebug() << ticker.symbol << ticker.lastPrice;
        });

        qDebug() << Q_FUNC_INFO << "created in:" << QThread::currentThread();
    }

    AppEngine::~AppEngine() {
        qDebug() << Q_FUNC_INFO << "launched from:" << QThread::currentThread();

        if (m_webSocket && m_webSocket->isOpen())
            m_webSocket->close();

        qDebug() << Q_FUNC_INFO << "finished";
    }

    bool AppEngine::hasRunned() {
        return m_webSocket && m_webSocket->isOpen();
    }

    void AppEngine::run(const QUrl& baseEndpoint) {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread() << "Socket thread:" << m_webSocket->thread();

        if (!m_webSocket)
            return;

        m_webSocket->open(baseEndpoint);
    }

    void AppEngine::stop() {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        if (!m_webSocket)
            return;

        if (m_webSocket->isOpen()) {
            m_webSocket->disconnectFromStream();
            m_webSocket->close();
        }
    }

    void AppEngine::addTrade(const QString& pair) {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        if (m_webSocket && m_webSocket->isOpen()) {
            m_webSocket->subscribeToStream(pair, {BybitWebSocket::Stream::Ticker});
            m_webSocket->connectToStream();
        }
    }

}