#include "BybitPrivateStreamer.hpp"
#include "Tools/Network/Bybit/BybitWebSocket.hpp"
#include "Handlers/Websocket/BybitWalletStreamHandler.hpp"
#include "Handlers/Websocket/BybitOrderStreamHandler.hpp"
#include "Handlers/Websocket/BybitExecutionStreamHandler.hpp"
#include "Handlers/Websocket/BybitPositionStreamHandler.hpp"
#include <QDateTime>

namespace Core::Markets
{

    BybitPrivateStreamer::BybitPrivateStreamer(QObject* parent)
        : BasePrivateStreamer(std::make_unique<Tools::BybitWebSocket>(
                                        Tools::SocketType::Private, Tools::MarketType::Unknown, parent), parent)
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        registerHandler<BybitWalletStreamHandler>();
        registerHandler<BybitOrderStreamHandler>();
        registerHandler<BybitExecutionStreamHandler>();
        registerHandler<BybitPositionStreamHandler>();
    }

    BybitPrivateStreamer::~BybitPrivateStreamer()
    {
        if (!m_webSocket)
            return;

        m_webSocket->close();
    }

    QString BybitPrivateStreamer::id()
    {
        return "private";
    }

    void BybitPrivateStreamer::init(const Tools::Api &api)
    {
        m_webSocket->init(api);
    }

    void BybitPrivateStreamer::sendSubscriptionMessage(const QStringList &streams)
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

    void BybitPrivateStreamer::sendUnsubscriptionMessage(const QStringList &streams)
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

    QString BybitPrivateStreamer::createWalletStream() const
    {
        return "wallet";
    }

    QString BybitPrivateStreamer::createOrderStream() const
    {
        return "order";
    }

    QString BybitPrivateStreamer::createPositionStream() const
    {
        return "position";
    }

    QString BybitPrivateStreamer::createExecutionStream() const
    {
        return "execution";
    }

    void BybitPrivateStreamer::onStarted()
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

    void BybitPrivateStreamer::onStopped()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        emit stopped(id());
    }

    void BybitPrivateStreamer::onPingMeasured(qint64 pingMs)
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        emit pingMeasured(pingMs);
    }

    void BybitPrivateStreamer::onErrorOccurred(const QString &error)
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        emit errorOccurred(id(), error);
    }

    bool BybitPrivateStreamer::isRunning()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        return m_webSocket && m_webSocket->isOpen();
    }

    void BybitPrivateStreamer::start()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        if (!m_webSocket)
            return;

        m_webSocket->open();
    }

    void BybitPrivateStreamer::stop()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        if (!m_webSocket)
            return;

        if (m_webSocket->isOpen())
            disconnectFromStreams();

        m_webSocket->close();
    }

    void BybitPrivateStreamer::restart()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        if (!m_webSocket)
            return;

        if (m_webSocket->isOpen())
            disconnectFromStreams();

        m_webSocket->reconnect();
    }

}