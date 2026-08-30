#include "BaseMarketDataStreamer.hpp"

namespace Core::Markets
{

    BaseMarketDataStreamer::BaseMarketDataStreamer(std::unique_ptr<Tools::BaseWebSocket> websocket, QObject *parent)
        : m_webSocket(std::move(websocket)), IMarketDataStreamer(parent)
    {
        connect(m_webSocket.get(), &Tools::BaseWebSocket::connected,
                this, &BaseMarketDataStreamer::onStarted, Qt::UniqueConnection);

        connect(m_webSocket.get(), &Tools::BaseWebSocket::disconnected,
                this, &BaseMarketDataStreamer::onStopped, Qt::UniqueConnection);

        connect(m_webSocket.get(), &Tools::BaseWebSocket::pingMeasured,
                this, &BaseMarketDataStreamer::onPingMeasured, Qt::UniqueConnection);

        connect(m_webSocket.get(), &Tools::BaseWebSocket::errorOccurred,
                this, &BaseMarketDataStreamer::onErrorOccurred, Qt::UniqueConnection);

        connect(m_webSocket.get(), &Tools::BaseWebSocket::messageReceived,
                this, &BaseMarketDataStreamer::onMessageReceived, Qt::UniqueConnection);
    }

    void BaseMarketDataStreamer::subscribeSymbol(const QString &symbol, QSet<WebSocketStreams> streams)
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        QStringList newStreams;

        for (auto stream : streams)
        {
            QString streamName = createStream(symbol, stream);

            if (!m_usedStreams.contains(streamName))
            {
                m_usedStreams.insert(streamName);
                newStreams << streamName;
            }
        }

        if (m_webSocket->isOpen() && !newStreams.isEmpty())
            sendSubscriptionMessage(newStreams);
    }

    void BaseMarketDataStreamer::unsubscribeSymbol(const QString &symbol, QSet<WebSocketStreams> streams)
    {
        QStringList streamsToRemove;

        for (auto stream : streams)
        {
            QString streamName = createStream(symbol, stream);

            if (m_usedStreams.contains(streamName))
            {
                m_usedStreams.remove(streamName);
                streamsToRemove << streamName;
            }
        }

        if (!streamsToRemove.isEmpty() && m_webSocket->isOpen())
            sendUnsubscriptionMessage(streamsToRemove);

    }

    void BaseMarketDataStreamer::connectToStreams()
    {
        if (!m_usedStreams.isEmpty())
            sendSubscriptionMessage(m_usedStreams.values());
    }

    void BaseMarketDataStreamer::disconnectFromStreams()
    {
        if (!m_usedStreams.isEmpty())
        {
            sendUnsubscriptionMessage(m_usedStreams.values());
            m_usedStreams.clear();
        }
    }

    QString BaseMarketDataStreamer::createStream(const QString& symbol, WebSocketStreams stream)
    {
        switch (stream)
        {
        case WebSocketStreams::Ticker:
            return createTickerStream(symbol);
        case WebSocketStreams::Orderbook:
            return createOrderbookStream(symbol);
        case WebSocketStreams::Kline:
            return createKlineStream(symbol);
        case WebSocketStreams::PublicTrade:
            return createPublicTradeStream(symbol);
        default:
            return QString();
        }
    }

}