#include "BybitPrivateDataStreamer.hpp"
#include "Tools/Network/Bybit/BybitWebSocket.hpp"
#include "Handlers/Websocket/BybitWalletStreamHandler.hpp"
#include "Handlers/Websocket/BybitOrderStreamHandler.hpp"
#include "Handlers/Websocket/BybitExecutionStreamHandler.hpp"
#include "Handlers/Websocket/BybitPositionStreamHandler.hpp"
#include <QDateTime>

namespace Core::Markets
{

    BybitPrivateDataStreamer::BybitPrivateDataStreamer(QObject* parent)
        : BasePrivateMarketDataStreamer(std::make_unique<Tools::BybitWebSocket>(
                                        Tools::SocketType::Private, Tools::MarketType::Unknown, parent), parent)
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        registerHandler<BybitWalletStreamHandler>();
        registerHandler<BybitOrderStreamHandler>();
        registerHandler<BybitExecutionStreamHandler>();
        registerHandler<BybitPositionStreamHandler>();
    }

    BybitPrivateDataStreamer::~BybitPrivateDataStreamer()
    {
        if (!m_webSocket)
            return;

        m_webSocket->close();
    }

    QString BybitPrivateDataStreamer::id()
    {
        return "private";
    }

    void BybitPrivateDataStreamer::setApi(const Tools::Api &api)
    {
        m_webSocket->init(api);
    }

    void BybitPrivateDataStreamer::sendSubscriptionMessage(const QStringList &streams)
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

    void BybitPrivateDataStreamer::sendUnsubscriptionMessage(const QStringList &streams)
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

    QString BybitPrivateDataStreamer::createWalletStream() const
    {
        return "wallet";
    }

    QString BybitPrivateDataStreamer::createOrderStream() const
    {
        return "order";
    }

    QString BybitPrivateDataStreamer::createPositionStream() const
    {
        return "position";
    }

    QString BybitPrivateDataStreamer::createExecutionStream() const
    {
        return "execution";
    }

    void BybitPrivateDataStreamer::onStarted()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        subscribe({
            Markets::PrivateStreams::Order,
            Markets::PrivateStreams::Position,
            Markets::PrivateStreams::Execution,
            Markets::PrivateStreams::Wallet
        });
        emit started(id());
    }

    void BybitPrivateDataStreamer::onStopped()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        emit stopped(id());
    }

    void BybitPrivateDataStreamer::onPingMeasured(qint64 pingMs)
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        emit pingMeasured(pingMs);
    }

    void BybitPrivateDataStreamer::onErrorOccurred(const QString &error)
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        emit errorOccurred(id(), error);
    }

    bool BybitPrivateDataStreamer::isRunning()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        return m_webSocket && m_webSocket->isOpen();
    }

    void BybitPrivateDataStreamer::start()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        if (!m_webSocket)
            return;

        m_webSocket->open();
    }

    void BybitPrivateDataStreamer::stop()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        if (!m_webSocket)
            return;

        if (m_webSocket->isOpen())
            disconnectFromStreams();

        m_webSocket->close();
    }

    void BybitPrivateDataStreamer::restart()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        if (!m_webSocket)
            return;

        if (m_webSocket->isOpen())
            disconnectFromStreams();

        m_webSocket->reconnect();
    }

}