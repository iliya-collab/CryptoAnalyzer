#include "BaseWebSocket.hpp"

namespace Core::Tools
{

    BaseWebSocket::BaseWebSocket(SocketType type, QObject *parent) : QObject(parent), m_type(type)
    {
        setupWebSocket();
        setupConnections();
    }

    BaseWebSocket::~BaseWebSocket()
    {
        close();
    }

    bool BaseWebSocket::isOpen()
    {
        return m_webSocket && (m_webSocket->state() == QAbstractSocket::ConnectedState);
    }

    void BaseWebSocket::sendMessage(const QString& message)
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        if (isOpen())
            m_webSocket->sendTextMessage(message);
    }

    void BaseWebSocket::open()
    {
        m_isManualClose = false;
        if (!isOpen() && m_webSocket->state() != QAbstractSocket::ConnectingState)
            m_webSocket->open(m_connectUrl);
    }

    void BaseWebSocket::close()
    {
        m_isManualClose = true;

        if (m_reconnectTimer)
            m_reconnectTimer->stop();

        if (m_pingTimer)
            m_pingTimer->stop();

        if (isOpen())
            m_webSocket->close();
    }

    void BaseWebSocket::reconnect()
    {
        if (m_reconnectTimer)
            m_reconnectTimer->stop();

        m_isManualClose = true;

        if (isOpen())
            m_webSocket->close();

        m_isManualClose = false;

        m_webSocket->open(m_connectUrl);
    }

    void BaseWebSocket::setupWebSocket()
    {
        QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
        sslConfig.setPeerVerifyMode(QSslSocket::VerifyPeer);
        sslConfig.setProtocol(QSsl::TlsV1_2OrLater);

        m_pingTimer = std::make_unique<QTimer>(this);
        m_webSocket = std::make_unique<QWebSocket>("", QWebSocketProtocol::VersionLatest, this);
        m_webSocket->setSslConfiguration(sslConfig);

        m_reconnectTimer = std::make_unique<QTimer>(this);
        m_reconnectTimer->setSingleShot(true);
    }

    void BaseWebSocket::setupConnections()
    {
        connect(m_webSocket.get(), &QWebSocket::connected, this, &BaseWebSocket::onConnected);
        connect(m_webSocket.get(), &QWebSocket::disconnected, this, [this]() {
            this->onDisconnected();
            if (!m_isManualClose)
                m_reconnectTimer->start(m_reconnectDelay);
        });
        connect(m_webSocket.get(), &QWebSocket::textMessageReceived, this, &BaseWebSocket::onTextMessageReceived);
        connect(m_webSocket.get(), &QWebSocket::bytesWritten, this, &BaseWebSocket::onBytesWritten);
        connect(m_webSocket.get(), &QWebSocket::errorOccurred, this, [this](QAbstractSocket::SocketError error) {
            this->onError(error);
            if (!m_isManualClose)
                m_reconnectTimer->start(m_reconnectDelay);
        });
        connect(m_webSocket.get(), &QWebSocket::sslErrors, this, &BaseWebSocket::onSslErrors);
        connect(m_pingTimer.get(), &QTimer::timeout, this, &BaseWebSocket::onPing);
        connect(m_reconnectTimer.get(), &QTimer::timeout, this, &BaseWebSocket::attemptReconnect);
    }

    void BaseWebSocket::attemptReconnect()
    {
        if (!m_isManualClose && !isOpen())
        {
            qDebug() << "Attempting to reconnect to" << m_connectUrl;
            m_webSocket->open(m_connectUrl);
        }
    }

}