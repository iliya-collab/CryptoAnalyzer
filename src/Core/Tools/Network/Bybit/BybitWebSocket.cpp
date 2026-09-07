#include "BybitWebSocket.hpp"
#include "BybitEndpointProvider.hpp"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QCryptographicHash>
#include <QMessageAuthenticationCode>

namespace Core::Tools {

    BybitWebSocket::BybitWebSocket(SocketType socketType, MarketType marketType, QObject* parent) : BaseWebSocket(socketType, parent) {}

    void BybitWebSocket::init(const Api &api)
    {
        setApi(api);
        setUrl(BybitEndpointProvider::webSocketUrl(m_socketType, m_marketType, api.m_isTestnet));
    }

    void BybitWebSocket::onConnected()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        if (m_socketType == SocketType::Private)
            sendAuthMessage();

        sendPingMessage();

        m_pingTimer->start(ACTIVE_PING_INTERVAL);
        emit connected();
    }

    void BybitWebSocket::onDisconnected()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        if (m_pingTimer && m_pingTimer->isActive())
            m_pingTimer->stop();

        emit disconnected();
    }

    void BybitWebSocket::onError(QAbstractSocket::SocketError error)
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        emit errorOccurred(m_webSocket->errorString());
    }

    void BybitWebSocket::onSslErrors(const QList<QSslError> &errors)
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        QStringList errorStrings;
        bool fatal = false;

        for (const QSslError& error : errors)
        {
            errorStrings << error.errorString();
            fatal = error.error() == QSslError::CertificateExpired ||
                    error.error() == QSslError::CertificateNotYetValid ||
                    error.error() == QSslError::CertificateUntrusted ||
                    error.error() == QSslError::HostNameMismatch;
        }

        emit errorOccurred("SSL errors: " + errorStrings.join(", "));

        if (fatal)
            m_webSocket->abort();
    }

    void BybitWebSocket::onTextMessageReceived(const QString &message)
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        auto jsonObj = parseTextMessage(message);
        if (jsonObj.has_value())
            processMessage(jsonObj.value());
        else
            emit errorOccurred(jsonObj.error());
    }

    void BybitWebSocket::onBytesWritten(qint64 bytes)
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
    }

    void BybitWebSocket::onPing()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        if (isOpen())
        {
            sendPingMessage();
            static int pingCounter = 0;
            if (++pingCounter % 10 == 0)
                cleanupPingTimestamps();
        }
    }

    void BybitWebSocket::sendAuthMessage()
    {
        qint64 currentMs = QDateTime::currentMSecsSinceEpoch();
        QString expires = QString::number(currentMs + 10000);

        QString signature = generateSignature(m_api.m_apiKey, m_api.m_secretKey, expires);

        QJsonObject authMessage;
        authMessage["op"] = "auth";

        QJsonArray args;
        args.append(m_api.m_apiKey);
        args.append(expires);
        args.append(signature);

        authMessage["args"] = args;
        authMessage["req_id"] = QString::number(QDateTime::currentMSecsSinceEpoch());

        sendMessage(QJsonDocument(authMessage).toJson(QJsonDocument::Compact));
    }

    void BybitWebSocket::sendPingMessage()
    {
        QJsonObject pingMessage;
        pingMessage["op"] = "ping";
        auto now = QDateTime::currentMSecsSinceEpoch();
        pingMessage["req_id"] = QString::number(now);

        m_pingTimestamps[QString::number(now)] = now;

        sendMessage(QJsonDocument(pingMessage).toJson());
    }

    QString BybitWebSocket::generateSignature(const QString &apiKey, const QString &apiSecret, const QString &expires)
    {
        QString signaturePayload = "GET/realtime" + expires;

        QMessageAuthenticationCode code(QCryptographicHash::Sha256);
        code.setKey(apiSecret.toUtf8());
        code.addData(signaturePayload.toUtf8());

        QByteArray hmacResult = code.result();
        return hmacResult.toHex();
    }

    void BybitWebSocket::cleanupPingTimestamps()
    {
        qint64 now = QDateTime::currentMSecsSinceEpoch();
        QMutableMapIterator<QString, qint64> it(m_pingTimestamps);
        while (it.hasNext())
        {
            it.next();
            if (now - it.value() > 10000)
                it.remove();
        }
    }

    void BybitWebSocket::processMessage(const QJsonObject &obj)
    {
        //QJsonDocument doc(obj);
        //qDebug() << doc.toJson(QJsonDocument::Compact);

        // Обработа ping
        if (obj.contains("op") && obj["op"].toString() == "ping")
        {
            QString reqId = obj["req_id"].toString();
            if (m_pingTimestamps.contains(reqId))
            {
                m_pingMs = QDateTime::currentMSecsSinceEpoch() - m_pingTimestamps[reqId];
                emit pingMeasured(m_pingMs);
                m_pingTimestamps.remove(reqId);
            }
            return;
        }

        // Обработка auth
        if (obj.contains("op") && obj["op"].toString() == "auth")
        {
            if (obj.contains("success") && obj["success"].toBool())
                emit authenticated();
            else
                emit errorOccurred(obj["ret_msg"].toString());
            return;
        }

        emit messageReceived(obj);
    }

    std::expected<QJsonObject, QString> BybitWebSocket::parseTextMessage(const QString &message)
    {
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8(), &parseError);

        if (parseError.error != QJsonParseError::NoError)
            return std::unexpected(parseError.errorString());

        if (!doc.isObject())
            return std::unexpected("Document is not an object");

        return doc.object();
    }

}