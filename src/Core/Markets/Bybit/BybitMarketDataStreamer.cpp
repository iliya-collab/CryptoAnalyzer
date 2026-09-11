#include "BybitMarketDataStreamer.hpp"
#include "Tools/Network/Bybit/BybitWebSocket.hpp"
#include "Handlers/Websocket/BybitTickerStreamHandler.hpp"
#include "Handlers/Websocket/BybitOrderbookStreamHandler.hpp"
#include "Handlers/Websocket/BybitKlineStreamHandler.hpp"
#include "Handlers/Websocket/BybitPublicTradeStreamHandler.hpp"
#include <QDateTime>

namespace Core::Markets
{

    BybitMarketDataStreamer::BybitMarketDataStreamer(Tools::MarketType type, QObject* parent)
        : BaseMarketDataStreamer(std::make_unique<Tools::BybitWebSocket>(
                                       Tools::SocketType::Public, type, parent), parent)
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        registerHandler<BybitTickerStreamHandler>();
        registerHandler<BybitOrderbookStreamHandler>();
        registerHandler<BybitKlineStreamHandler>();
        registerHandler<BybitPublicTradeStreamHandler>();
    }

    BybitMarketDataStreamer::~BybitMarketDataStreamer()
    {
        if (!m_webSocket)
            return;

        m_webSocket->close();
    }

    QString BybitMarketDataStreamer::id()
    {
        return m_webSocket->getId();
    }

    void BybitMarketDataStreamer::init(bool isTestnet)
    {
        Tools::Api api {"", "", isTestnet};
        m_webSocket->init(api);
    }

    void BybitMarketDataStreamer::sendSubscriptionMessage(const QStringList &streams)
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

    void BybitMarketDataStreamer::sendUnsubscriptionMessage(const QStringList &streams)
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

    QString BybitMarketDataStreamer::createTickerStream(const QString &symbol) const
    {
        return QString("tickers.%1").arg(symbol);
    }

    QString BybitMarketDataStreamer::createOrderbookStream(const QString &symbol) const
    {
        return QString("orderbook.50.%1").arg(symbol);
    }

    QString BybitMarketDataStreamer::createKlineStream(const QString &symbol) const
    {
        return QString("kline.1.%1").arg(symbol);
    }

    QString BybitMarketDataStreamer::createPublicTradeStream(const QString &symbol) const
    {
        return QString("publicTrade.%1").arg(symbol);
    }

    void BybitMarketDataStreamer::onStarted()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        if (!m_lastPair.isEmpty())
        {
            subscribeSymbol(m_lastPair, {
                                            Markets::PublicStreams::Ticker,
                                            Markets::PublicStreams::Orderbook,
                                            Markets::PublicStreams::Kline,
                                            Markets::PublicStreams::PublicTrade
                                        });
        }
        emit started(id());
    }

    void BybitMarketDataStreamer::onStopped()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        emit stopped(id());
    }

    void BybitMarketDataStreamer::onPingMeasured(qint64 pingMs)
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        emit pingMeasured(pingMs);
    }

    void BybitMarketDataStreamer::onErrorOccurred(const QString &error)
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        emit errorOccurred(id(), error);
    }

    bool BybitMarketDataStreamer::isRunning()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        return m_webSocket && m_webSocket->isOpen();
    }

    void BybitMarketDataStreamer::start()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        if (!m_webSocket)
            return;

        m_webSocket->open();
    }

    void BybitMarketDataStreamer::stop()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        if (!m_webSocket)
            return;

        if (m_webSocket->isOpen())
            disconnectFromStreams();

        m_webSocket->close();
    }

    void BybitMarketDataStreamer::restart()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        if (!m_webSocket)
            return;

        if (m_webSocket->isOpen())
            disconnectFromStreams();

        m_webSocket->reconnect();
    }

}