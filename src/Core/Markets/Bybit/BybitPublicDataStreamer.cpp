#include "BybitPublicDataStreamer.hpp"
#include "Tools/Network/Bybit/BybitWebSocket.hpp"
#include "Handlers/Websocket/BybitTickerStreamHandler.hpp"
#include "Handlers/Websocket/BybitOrderbookStreamHandler.hpp"
#include "Handlers/Websocket/BybitKlineStreamHandler.hpp"
#include "Handlers/Websocket/BybitPublicTradeStreamHandler.hpp"
#include <QDateTime>

namespace Core::Markets
{

    BybitPublicDataStreamer::BybitPublicDataStreamer(Tools::MarketType type, QObject* parent)
        : BasePublicMarketDataStreamer(std::make_unique<Tools::BybitWebSocket>(
                                       Tools::SocketType::Public, type, parent), parent)
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        registerHandler<BybitTickerStreamHandler>();
        registerHandler<BybitOrderbookStreamHandler>();
        registerHandler<BybitKlineStreamHandler>();
        registerHandler<BybitPublicTradeStreamHandler>();
    }

    BybitPublicDataStreamer::~BybitPublicDataStreamer()
    {
        if (!m_webSocket)
            return;

        m_webSocket->close();
    }

    QString BybitPublicDataStreamer::id()
    {
        return m_webSocket->getId();
    }

    void BybitPublicDataStreamer::setApi(const Tools::Api &api)
    {
        m_webSocket->init(api);
    }

    void BybitPublicDataStreamer::sendSubscriptionMessage(const QStringList &streams)
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

    void BybitPublicDataStreamer::sendUnsubscriptionMessage(const QStringList &streams)
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

    QString BybitPublicDataStreamer::createTickerStream(const QString &symbol) const
    {
        return QString("tickers.%1").arg(symbol);
    }

    QString BybitPublicDataStreamer::createOrderbookStream(const QString &symbol) const
    {
        return QString("orderbook.50.%1").arg(symbol);
    }

    QString BybitPublicDataStreamer::createKlineStream(const QString &symbol) const
    {
        return QString("kline.1.%1").arg(symbol);
    }

    QString BybitPublicDataStreamer::createPublicTradeStream(const QString &symbol) const
    {
        return QString("publicTrade.%1").arg(symbol);
    }

    void BybitPublicDataStreamer::onStarted()
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

    void BybitPublicDataStreamer::onStopped()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        emit stopped(id());
    }

    void BybitPublicDataStreamer::onPingMeasured(qint64 pingMs)
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        emit pingMeasured(pingMs);
    }

    void BybitPublicDataStreamer::onErrorOccurred(const QString &error)
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        emit errorOccurred(id(), error);
    }

    bool BybitPublicDataStreamer::isRunning()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        return m_webSocket && m_webSocket->isOpen();
    }

    void BybitPublicDataStreamer::start()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        if (!m_webSocket)
            return;

        m_webSocket->open();
    }

    void BybitPublicDataStreamer::stop()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        if (!m_webSocket)
            return;

        if (m_webSocket->isOpen())
            disconnectFromStreams();

        m_webSocket->close();
    }

    void BybitPublicDataStreamer::restart()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        if (!m_webSocket)
            return;

        if (m_webSocket->isOpen())
            disconnectFromStreams();

        m_webSocket->reconnect();
    }

}