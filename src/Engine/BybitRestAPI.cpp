#include "Engine/BybitRestAPI.hpp"

#include <QCryptographicHash>
#include <QMessageAuthenticationCode>
#include <QDate>
#include <QTimer>

Engine::BybitRestAPI::BybitRestAPI(const API& api, QObject* parent) :
    m_api(api), 
    IRestAPI(parent) 
{
    initBaseEndpoint();
}

void Engine::BybitRestAPI::initBaseEndpoint() {
    m_baseEndpoint = m_api.testnet ? QString("https://api-testnet.bybit.com") : QString("https://api.bybit.com");
    qDebug() << "Base endpoint set to:" << m_baseEndpoint;
}

Engine::BybitRestAPI::APIHeaders Engine::BybitRestAPI::initAPIHeaders(const QString& queryString) {
    APIHeaders headers;
    headers.X_BAPI_API_KEY = m_api.api_key;
    headers.X_BAPI_TIMESTAMP = QString::number(QDateTime::currentMSecsSinceEpoch());
    headers.X_BAPI_RECV_WINDOW = "500";
    headers.X_BAPI_SIGN = generateSignature(headers.X_BAPI_TIMESTAMP, headers.X_BAPI_RECV_WINDOW, queryString);
    return headers;
}

void Engine::BybitRestAPI::requestEndpoint(const QString& endpoint, const QUrlQuery& params, int timeout) {
    QString urlString = m_baseEndpoint + endpoint;
    QUrl url(urlString);
    
    if (!params.isEmpty())
        url.setQuery(params);
    
    qDebug() << "Request URL:" << url.toString(QUrl::FullyEncoded);
    
    QNetworkRequest request(url);
    
    QString queryString = url.query(QUrl::FullyEncoded);
    APIHeaders headers = initAPIHeaders(queryString);
    
    request.setRawHeader("X-BAPI-SIGN", headers.X_BAPI_SIGN.toUtf8());
    request.setRawHeader("X-BAPI-API-KEY", headers.X_BAPI_API_KEY.toUtf8());
    request.setRawHeader("X-BAPI-TIMESTAMP", headers.X_BAPI_TIMESTAMP.toUtf8());
    request.setRawHeader("X-BAPI-RECV-WINDOW", headers.X_BAPI_RECV_WINDOW.toUtf8());

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0");

    if (timeout > 0)
        request.setTransferTimeout(timeout);
    
    QNetworkReply* reply = m_manager->get(request);
    
    connect(reply, &QNetworkReply::finished, this, &Engine::BybitRestAPI::handleResponse);
    connect(reply, &QNetworkReply::downloadProgress, this, [reply] (qint64 bytesReceived, qint64 bytesTotal) {
        qDebug() << "Download progress:" << bytesReceived << "/" << bytesTotal << "URL:" << reply->url().toString();
    });
}

QString Engine::BybitRestAPI::generateSignature(const QString& timestamp, const QString& recv_window, const QString& queryString) {
    QString dataForSign = timestamp + m_api.api_key + recv_window + queryString;

    QMessageAuthenticationCode hmac(QCryptographicHash::Sha256);
    hmac.setKey(m_api.secret_key.toUtf8());
    hmac.addData(dataForSign.toUtf8());

    return QString(hmac.result().toHex());
}