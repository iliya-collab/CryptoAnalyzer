#include "Engine/CryptoCompare.hpp"

const QString Engine::CryptoCompare::baseUrl = QString("https://www.cryptocompare.com");

void Engine::CryptoCompare::requestEndpoint(const QString& endpoint, const QUrlQuery& params, int timeout) {
    QString urlString = m_baseEndpoint + endpoint;
    QUrl url(urlString);
    
    if (!params.isEmpty())
        url.setQuery(params);
    
    if (!url.isValid()) {
        qDebug() << "Invalid URL:" << urlString;
        emit errorOccurred("Invalid URL: " + urlString);
        return;
    }
    
    qDebug() << "Request URL:" << url.toString(QUrl::FullyEncoded);
    
    QNetworkRequest request(url);
    
    request.setTransferTimeout(timeout);
    
    QNetworkReply* reply = m_manager->get(request);
    
    connect(reply, &QNetworkReply::finished, this, &Engine::CryptoCompare::handleResponse);
    connect(reply, &QNetworkReply::downloadProgress, this, [reply] (qint64 bytesReceived, qint64 bytesTotal) {
        qDebug() << "Download progress:" << bytesReceived << "/" << bytesTotal;
    });
}