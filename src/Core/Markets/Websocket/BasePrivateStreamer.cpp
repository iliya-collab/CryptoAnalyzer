#include "BasePrivateStreamer.hpp"

namespace Core::Markets
{

    BasePrivateStreamer::BasePrivateStreamer(std::unique_ptr<Tools::BaseWebSocket> websocket, QObject *parent)
        : m_webSocket(std::move(websocket)), IPrivateStreamer(parent)
    {
        connect(m_webSocket.get(), &Tools::BaseWebSocket::connected,
                this, &BasePrivateStreamer::onStarted, Qt::UniqueConnection);

        connect(m_webSocket.get(), &Tools::BaseWebSocket::disconnected,
                this, &BasePrivateStreamer::onStopped, Qt::UniqueConnection);

        connect(m_webSocket.get(), &Tools::BaseWebSocket::pingMeasured,
                this, &BasePrivateStreamer::onPingMeasured, Qt::UniqueConnection);

        connect(m_webSocket.get(), &Tools::BaseWebSocket::errorOccurred,
                this, &BasePrivateStreamer::onErrorOccurred, Qt::UniqueConnection);

        connect(m_webSocket.get(), &Tools::BaseWebSocket::messageReceived,
                this, &BasePrivateStreamer::onMessageReceived, Qt::UniqueConnection);
    }

    void BasePrivateStreamer::subscribe(QSet<PrivateStreams> streams)
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        QStringList newStreams;

        for (auto stream : streams)
        {
            QString streamName = createStream(stream);

            if (!m_usedStreams.contains(streamName))
            {
                m_usedStreams.insert(streamName);
                newStreams << streamName;
            }
        }

        if (m_webSocket->isOpen() && !newStreams.isEmpty())
            sendSubscriptionMessage(newStreams);
    }

    void BasePrivateStreamer::unsubscribe(QSet<PrivateStreams> streams)
    {
        QStringList streamsToRemove;

        for (auto stream : streams)
        {
            QString streamName = createStream(stream);

            if (m_usedStreams.contains(streamName))
            {
                m_usedStreams.remove(streamName);
                streamsToRemove << streamName;
            }
        }

        if (!streamsToRemove.isEmpty() && m_webSocket->isOpen())
            sendUnsubscriptionMessage(streamsToRemove);

    }

    QString BasePrivateStreamer::createStream(PrivateStreams stream)
    {
        switch (stream)
        {
        case PrivateStreams::Wallet:
            return createWalletStream();
        case PrivateStreams::Order:
            return createOrderStream();
        case PrivateStreams::Execution:
            return createExecutionStream();
        case PrivateStreams::Position:
            return createExecutionStream();
        default:
            return QString();
        }
    }

    void BasePrivateStreamer::connectToStreams()
    {
        if (!m_usedStreams.isEmpty())
            sendSubscriptionMessage(m_usedStreams.values());
    }

    void BasePrivateStreamer::disconnectFromStreams()
    {
        if (!m_usedStreams.isEmpty())
        {
            sendUnsubscriptionMessage(m_usedStreams.values());
            m_usedStreams.clear();
        }
    }

}