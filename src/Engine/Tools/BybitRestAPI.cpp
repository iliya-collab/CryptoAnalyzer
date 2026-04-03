#include "Engine/Tools/BybitRestAPI.hpp"

#include <QCryptographicHash>
#include <QMessageAuthenticationCode>
#include <QDate>
#include <QTimer>

Engine::BybitRestAPI::BybitRestAPI(const QString& api_key, const QString& secret_key, bool is_testnet, QObject* parent) : 
    m_apiKey(api_key), 
    m_secretKey(secret_key),
    QObject(parent) 
{
    m_manager = new QNetworkAccessManager(parent);
    initBaseEndpoint(is_testnet);
}

void Engine::BybitRestAPI::handleResponse() {
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

void Engine::BybitRestAPI::initBaseEndpoint(bool is_testnet) {
    m_baseEndpoint = is_testnet ? QString("https://api-testnet.bybit.com") : QString("https://api.bybit.com");
}

Engine::BybitRestAPI::APIHeaders Engine::BybitRestAPI::initAPIHeaders(const QString& queryString) {
    APIHeaders headers;
    headers.X_BAPI_API_KEY = m_apiKey;
    headers.X_BAPI_TIMESTAMP = QString::number(QDateTime::currentMSecsSinceEpoch());
    headers.X_BAPI_RECV_WINDOW = "500";
    headers.X_BAPI_SIGN = generateSignature(headers.X_BAPI_TIMESTAMP, headers.X_BAPI_RECV_WINDOW, queryString);
    return headers;
}

void Engine::BybitRestAPI::addAPIHeaders(const QUrl& url,QNetworkRequest& request) {
    QString queryString = url.query(QUrl::FullyEncoded);
    APIHeaders headers = initAPIHeaders(queryString);
    request.setRawHeader("X-BAPI-SIGN", headers.X_BAPI_SIGN.toUtf8());
    request.setRawHeader("X-BAPI-API-KEY", headers.X_BAPI_API_KEY.toUtf8());
    request.setRawHeader("X-BAPI-TIMESTAMP", headers.X_BAPI_TIMESTAMP.toUtf8());
    request.setRawHeader("X-BAPI-RECV-WINDOW", headers.X_BAPI_RECV_WINDOW.toUtf8());
}

void Engine::BybitRestAPI::requestEndpoint(const QString& endpoint, const QUrlQuery& params, int timeout) {
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

QString Engine::BybitRestAPI::generateSignature(const QString& timestamp, const QString& recv_window, const QString& queryString) {
    QString dataForSign = timestamp + m_apiKey + recv_window + queryString;

    QMessageAuthenticationCode hmac(QCryptographicHash::Sha256);
    hmac.setKey(m_secretKey.toUtf8());
    hmac.addData(dataForSign.toUtf8());

    return QString(hmac.result().toHex());
}