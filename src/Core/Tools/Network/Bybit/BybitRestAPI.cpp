#include "BybitRestAPI.hpp"

#include <QCryptographicHash>
#include <QMessageAuthenticationCode>
#include <QDate>
#include <QTimer>

namespace Core::Tools {

    BybitRestAPI::BybitRestAPI(QObject* parent) : BaseRestAPI(parent)
    {
        m_manager = new QNetworkAccessManager(parent);
    }

    void BybitRestAPI::initApi(const Api& api)
    {
        m_api = api;
        m_baseEndpoint = m_api.m_isTestnet ? "https://api-testnet.bybit.com" : "https://api.bybit.com";
    }

    void BybitRestAPI::onHandleResponse()
    {
        QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

        if (!reply)
            return;

        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (statusCode != 200) {
            emit errorOccurred(QString("HTTP error %1").arg(statusCode));
            reply->deleteLater();
            return;
        }

        if (reply->error() == QNetworkReply::NoError) {
            QByteArray response = reply->readAll();
            if (!response.isNull())
                emit dataReceived(reply->url(), response);
            else
                emit errorOccurred("Failed to parse JSON response");
        }
        else
            emit errorOccurred(reply->errorString());

        reply->deleteLater();
    }

    BybitRestAPI::APIHeaders BybitRestAPI::initAPIHeaders(const QString& queryString)
    {
        APIHeaders headers;
        headers.X_BAPI_API_KEY = m_api.m_apiKey;
        headers.X_BAPI_TIMESTAMP = QString::number(QDateTime::currentMSecsSinceEpoch());
        headers.X_BAPI_RECV_WINDOW = "5000";
        headers.X_BAPI_SIGN = generateSignature(headers.X_BAPI_TIMESTAMP, headers.X_BAPI_RECV_WINDOW, queryString);
        return headers;
    }

    void BybitRestAPI::addAPIHeaders(const QUrl& url,QNetworkRequest& request)
    {
        QUrlQuery sortedQuery(url);
        sortedQuery.setQueryItems(sortedQuery.queryItems());
        QString queryString = sortedQuery.toString(QUrl::FullyEncoded);

        APIHeaders headers = initAPIHeaders(queryString);
        request.setRawHeader("X-BAPI-SIGN", headers.X_BAPI_SIGN.toUtf8());
        request.setRawHeader("X-BAPI-API-KEY", headers.X_BAPI_API_KEY.toUtf8());
        request.setRawHeader("X-BAPI-TIMESTAMP", headers.X_BAPI_TIMESTAMP.toUtf8());
        request.setRawHeader("X-BAPI-RECV-WINDOW", headers.X_BAPI_RECV_WINDOW.toUtf8());
    }

    QUrl BybitRestAPI::requestEndpoint(const QString& endpoint, const QUrlQuery& params, int timeout)
    {
        QString urlString = m_baseEndpoint + endpoint;
        QUrl url(urlString);

        if (!params.isEmpty())
            url.setQuery(params);

        //qInfo() << "Request URL:" << url.toString(QUrl::FullyEncoded);

        QNetworkRequest request(url);

        addAPIHeaders(url, request);

        request.setRawHeader("Connection", "keep-alive");
        request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        request.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0");

        if (timeout > 0)
            request.setTransferTimeout(timeout);

        QNetworkReply* reply = m_manager->get(request);

        connect(reply, &QNetworkReply::finished, this, &BybitRestAPI::onHandleResponse, Qt::UniqueConnection);
        connect(reply, &QNetworkReply::downloadProgress, this, &BybitRestAPI::downloadProgress, Qt::UniqueConnection);

        return request.url();
    }

    QString BybitRestAPI::generateSignature(const QString& timesTamp, const QString& recvWindow, const QString& queryString)
    {
        QString dataForSign = timesTamp + m_api.m_apiKey + recvWindow + queryString;

        QMessageAuthenticationCode hmac(QCryptographicHash::Sha256);
        hmac.setKey(m_api.m_secretKey.toUtf8());
        hmac.addData(dataForSign.toUtf8());

        return QString(hmac.result().toHex());
    }

}