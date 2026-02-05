#include "Engine/BybitRestAPI.hpp"

#include <QCryptographicHash>
#include <QMessageAuthenticationCode>
#include <QDate>

Engine::BybitRestAPI::BybitRestAPI(const QString& apiKey, const QString& secretKey, bool testNet, QObject* parent) : 
    QObject(parent), 
    m_api_key(apiKey), 
    m_secret_key(secretKey),
    m_testnet(testNet) 
{
    m_manager = new QNetworkAccessManager(parent);
    initBaseEndpoint();
}

void Engine::BybitRestAPI::initBaseEndpoint() {
    m_baseEndpoint = m_testnet ? "https://api-testnet.bybit.com" : "https://api.bybit.com";
}

Engine::BybitRestAPI::APIHeaders Engine::BybitRestAPI::initAPIHeaders(const QString& params) {
    APIHeaders headers;
    headers.X_BAPI_API_KEY = m_api_key;
    headers.X_BAPI_TIMESTAMP = QString::number(QDateTime::currentMSecsSinceEpoch());
    headers.X_BAPI_RECV_WINDOW = "500";
    headers.X_BAPI_SIGN = generateSignature(headers.X_BAPI_TIMESTAMP, headers.X_BAPI_RECV_WINDOW, params);
    return headers;
}

void Engine::BybitRestAPI::requestEndpoint(const QString& endpoint, const QString& params) {
    QUrl url = QUrl(m_baseEndpoint + endpoint);
    QNetworkRequest request(url);

    APIHeaders headers = initAPIHeaders(params);

    request.setRawHeader("X-BAPI-SIGN", headers.X_BAPI_SIGN.toUtf8());
    request.setRawHeader("X-BAPI-API-KEY", headers.X_BAPI_API_KEY.toUtf8());
    request.setRawHeader("X-BAPI-TIMESTAMP", headers.X_BAPI_TIMESTAMP.toUtf8());
    request.setRawHeader("X-BAPI-RECV-WINDOW", headers.X_BAPI_RECV_WINDOW.toUtf8());
    request.setRawHeader("Content-Type", "application/json");

    QNetworkReply *reply = m_manager->get(request);
    
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onRequestFinished(reply);
    });

}

std::expected<QJsonObject, QString> Engine::BybitRestAPI::getReply() {
    return m_reply;
}

QString Engine::BybitRestAPI::generateSignature(const QString& timestamp, const QString& recv_window, const QString& params) {
    QString dataForSign = timestamp + m_api_key + recv_window + params;

    QMessageAuthenticationCode hmac(QCryptographicHash::Sha256);
    hmac.setKey(m_secret_key.toUtf8());
    hmac.addData(dataForSign.toUtf8());

    return QString(hmac.result().toHex());
}

std::expected<QJsonObject, QString> Engine::BybitRestAPI::onRequestFinished(QNetworkReply* reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray response = reply->readAll();
        //qDebug() << "Raw response:" << response;
        
        QJsonDocument doc = QJsonDocument::fromJson(response);
        if (!doc.isNull()) {
            QJsonObject obj = doc.object();
            return obj;
        } else {
            return std::unexpected<QString>("Failed to parse JSON response");
        }
    } else {
        return std::unexpected<QString>(reply->errorString());
    }
    reply->deleteLater();
}
