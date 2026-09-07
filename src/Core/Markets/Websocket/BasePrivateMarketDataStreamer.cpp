#include "BasePrivateMarketDataStreamer.hpp"

namespace Core::Markets
{

    BasePrivateMarketDataStreamer::BasePrivateMarketDataStreamer(std::unique_ptr<Tools::BaseWebSocket> websocket, QObject *parent)
        : m_webSocket(std::move(websocket)), IPrivateMarketDataStreamer(parent)
    {
        connect(m_webSocket.get(), &Tools::BaseWebSocket::connected,
                this, &BasePrivateMarketDataStreamer::onStarted, Qt::UniqueConnection);

        connect(m_webSocket.get(), &Tools::BaseWebSocket::disconnected,
                this, &BasePrivateMarketDataStreamer::onStopped, Qt::UniqueConnection);

        connect(m_webSocket.get(), &Tools::BaseWebSocket::pingMeasured,
                this, &BasePrivateMarketDataStreamer::onPingMeasured, Qt::UniqueConnection);

        connect(m_webSocket.get(), &Tools::BaseWebSocket::errorOccurred,
                this, &BasePrivateMarketDataStreamer::onErrorOccurred, Qt::UniqueConnection);

        connect(m_webSocket.get(), &Tools::BaseWebSocket::messageReceived,
                this, &BasePrivateMarketDataStreamer::onMessageReceived, Qt::UniqueConnection);
    }

    void BasePrivateMarketDataStreamer::subscribe(QSet<PrivateStreams> streams)
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

    void BasePrivateMarketDataStreamer::unsubscribe(QSet<PrivateStreams> streams)
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

    QString BasePrivateMarketDataStreamer::createStream(PrivateStreams stream)
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

    void BasePrivateMarketDataStreamer::connectToStreams()
    {
        if (!m_usedStreams.isEmpty())
            sendSubscriptionMessage(m_usedStreams.values());
    }

    void BasePrivateMarketDataStreamer::disconnectFromStreams()
    {
        if (!m_usedStreams.isEmpty())
        {
            sendUnsubscriptionMessage(m_usedStreams.values());
            m_usedStreams.clear();
        }
    }

}