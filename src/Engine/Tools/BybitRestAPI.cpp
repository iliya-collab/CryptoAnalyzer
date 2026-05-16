#include "Engine/Tools/BybitRestAPI.hpp"

#include <QCryptographicHash>
#include <QMessageAuthenticationCode>
#include <QDate>
#include <QTimer>

namespace Engine {

    BybitRestAPI::BybitRestAPI(QObject* parent) : QObject(parent) {
        m_manager = new QNetworkAccessManager(parent);
    }

    void BybitRestAPI::initAPI(const API& api) {
        m_api = api;
        m_baseEndpoint = m_api.m_isTestnet ? "https://api-testnet.bybit.com" : "https://api.bybit.com";
    }

    void BybitRestAPI::handleResponse() {
        QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

        if (!reply)
            return;

        if (reply->error() == QNetworkReply::NoError) {
            QByteArray response = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(response);
            if (!doc.isNull())
                emit dataReceived(doc.object());
            else
                emit errorOccurred("Failed to parse JSON response");
        }
        else
            emit errorOccurred(reply->errorString());

        reply->deleteLater();
    }

    BybitRestAPI::APIHeaders BybitRestAPI::initAPIHeaders(const QString& queryString) {
        APIHeaders headers;
        headers.X_BAPI_API_KEY = m_api.m_apiKey;
        headers.X_BAPI_TIMESTAMP = QString::number(QDateTime::currentMSecsSinceEpoch());
        headers.X_BAPI_RECV_WINDOW = "500";
        headers.X_BAPI_SIGN = generateSignature(headers.X_BAPI_TIMESTAMP, headers.X_BAPI_RECV_WINDOW, queryString);
        return headers;
    }

    void BybitRestAPI::addAPIHeaders(const QUrl& url,QNetworkRequest& request) {
        QString queryString = url.query(QUrl::FullyEncoded);
        APIHeaders headers = initAPIHeaders(queryString);
        request.setRawHeader("X-BAPI-SIGN", headers.X_BAPI_SIGN.toUtf8());
        request.setRawHeader("X-BAPI-API-KEY", headers.X_BAPI_API_KEY.toUtf8());
        request.setRawHeader("X-BAPI-TIMESTAMP", headers.X_BAPI_TIMESTAMP.toUtf8());
        request.setRawHeader("X-BAPI-RECV-WINDOW", headers.X_BAPI_RECV_WINDOW.toUtf8());
    }

    void BybitRestAPI::requestEndpoint(const QString& endpoint, const QUrlQuery& params, int timeout) {
        QString urlString = m_baseEndpoint + endpoint;
        QUrl url(urlString);

        if (!params.isEmpty())
            url.setQuery(params);

        //qDebug() << "Request URL:" << url.toString(QUrl::FullyEncoded);

        QNetworkRequest request(url);

        addAPIHeaders(url, request);

        request.setRawHeader("Connection", "keep-alive");
        request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        request.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0");

        if (timeout > 0)
            request.setTransferTimeout(timeout);

        QNetworkReply* reply = m_manager->get(request);

        connect(reply, &QNetworkReply::finished, this, &Engine::BybitRestAPI::handleResponse);
        connect(reply, &QNetworkReply::downloadProgress, this, &Engine::BybitRestAPI::downloadProgress);
    }

    QString BybitRestAPI::generateSignature(const QString& timesTamp, const QString& recvWindow, const QString& queryString) {
        QString dataForSign = timesTamp + m_api.m_apiKey + recvWindow + queryString;

        QMessageAuthenticationCode hmac(QCryptographicHash::Sha256);
        hmac.setKey(m_api.m_secretKey.toUtf8());
        hmac.addData(dataForSign.toUtf8());

        return QString(hmac.result().toHex());
    }

}