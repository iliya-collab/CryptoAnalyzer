#include "BybitDataStreamer.hpp"
#include "Tools/Network/Bybit/BybitWebSocket.hpp"
#include "Handlers/Websocket/BybitTickerStreamHandler.hpp"
#include "Handlers/Websocket/BybitOrderbookStreamHandler.hpp"
#include "Handlers/Websocket/BybitKlineStreamHandler.hpp"
#include "Handlers/Websocket/BybitPublicTradeStreamHandler.hpp"
#include <QDateTime>

namespace Core::Markets
{

    BybitDataStreamer::BybitDataStreamer(QObject* parent)
        : BaseMarketDataStreamer(std::make_unique<Tools::BybitWebSocket>(Tools::BaseWebSocket::SocketType::Public, parent), parent)
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        registerHandler<BybitTickerStreamHandler>("tickers.");
        registerHandler<BybitOrderbookStreamHandler>("orderbook.");
        registerHandler<BybitKlineStreamHandler>("kline.");
        registerHandler<BybitPublicTradeStreamHandler>("publicTrade.");
    }

    BybitDataStreamer::~BybitDataStreamer()
    {
        if (!m_webSocket)
            return;

        m_webSocket->close();
    }

    void BybitDataStreamer::sendSubscriptionMessage(const QStringList &streams)
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        for (int i = 0; i < streams.size(); i += MAX_STREAMS_PER_SUBSCRIPTION)
        {
            QStringList chunk = streams.mid(i, MAX_STREAMS_PER_SUBSCRIPTION);

            QJsonObject subscribeMessage;
            subscribeMessage["op"] = "subscribe";
            subscribeMessage["args"] = QJsonArray::fromStringList(chunk);
            subscribeMessage["req_id"] = QString::number(QDateTime::currentMSecsSinceEpoch());

            QJsonDocument doc(subscribeMessage);
            QString message = doc.toJson(QJsonDocument::Compact);

            qDebug() << message;

            m_webSocket->sendMessage(message);

            if (i + MAX_STREAMS_PER_SUBSCRIPTION < streams.size())
                QThread::msleep(100);
        }
    }

    void BybitDataStreamer::sendUnsubscriptionMessage(const QStringList &streams)
    {
        for (int i = 0; i < streams.size(); i += MAX_STREAMS_PER_SUBSCRIPTION)
        {
            QStringList chunk = streams.mid(i, MAX_STREAMS_PER_SUBSCRIPTION);

            QJsonObject unsubscribeMessage;
            unsubscribeMessage["op"] = "unsubscribe";
            unsubscribeMessage["args"] = QJsonArray::fromStringList(chunk);
            unsubscribeMessage["req_id"] = QString::number(QDateTime::currentMSecsSinceEpoch());

            QJsonDocument doc(unsubscribeMessage);
            QString message = doc.toJson(QJsonDocument::Compact);

            m_webSocket->sendMessage(message);

            if (i + MAX_STREAMS_PER_SUBSCRIPTION < streams.size())
                QThread::msleep(100);
        }
    }

    QString BybitDataStreamer::createTickerStream(const QString &symbol) const
    {
        return QString("tickers.%1").arg(symbol);
    }

    QString BybitDataStreamer::createOrderbookStream(const QString &symbol) const
    {
        return QString("orderbook.50.%1").arg(symbol);
    }

    QString BybitDataStreamer::createKlineStream(const QString &symbol) const
    {
        return QString("kline.1.%1").arg(symbol);
    }

    QString BybitDataStreamer::createPublicTradeStream(const QString &symbol) const
    {
        return QString("publicTrade.%1").arg(symbol);
    }

    void BybitDataStreamer::onStarted()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        if (!m_lastPair.isEmpty())
        {
            subscribeSymbol(m_lastPair, {
                                            Markets::WebSocketStreams::Ticker,
                                            Markets::WebSocketStreams::Orderbook,
                                            Markets::WebSocketStreams::Kline,
                                            Markets::WebSocketStreams::PublicTrade
                                        });
        }
        emit started();
    }

    void BybitDataStreamer::onStopped()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        emit stopped();
    }

    void BybitDataStreamer::onPingMeasured(qint64 pingMs)
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        emit pingMeasured(pingMs);
    }

    void BybitDataStreamer::onErrorOccurred(const QString &error)
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        emit errorOccurred(error);
    }

    void BybitDataStreamer::onMessageReceived(const QJsonObject &message)
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        if (!message.contains("topic"))
            return;

        QString topic = message["topic"].toString();
        for (auto it = m_handlers.begin(); it != m_handlers.end(); ++it)
        {
            if (topic.startsWith(it->first))
            {
                it->second->handle(message, this);
                break;
            }
        }
    }

    bool BybitDataStreamer::hasRunned()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        return m_webSocket && m_webSocket->isOpen();
    }

    void BybitDataStreamer::setApi(const Tools::Api& api)
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        if (!m_webSocket)
            return;

        m_webSocket->initApi(api);
    }

    void BybitDataStreamer::start()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        if (!m_webSocket)
            return;

        m_webSocket->open();
    }

    void BybitDataStreamer::stop()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        if (!m_webSocket)
            return;

        if (m_webSocket->isOpen())
            disconnectFromStreams();

        m_webSocket->close();
    }

    void BybitDataStreamer::restart()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        if (!m_webSocket)
            return;

        if (m_webSocket->isOpen())
            disconnectFromStreams();

        m_webSocket->reconnect();
    }

}