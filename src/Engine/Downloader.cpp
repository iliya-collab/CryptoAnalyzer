#include "Engine/Downloader.hpp"

Engine::Downloader::Downloader(QObject* parent) : QObject(parent) {
    m_manager = new QNetworkAccessManager(this);
    connect(m_manager, &QNetworkAccessManager::finished, this, &Downloader::onDownloadFinished);
}

void Engine::Downloader::download(const QString& urlResource) {
    QUrl url(urlResource);
    
    qDebug() << "Loading a resource:" << url.toString(QUrl::FullyEncoded);
    
    QNetworkRequest request(url);

    QNetworkReply* reply = m_manager->get(request);
    
    connect(reply, &QNetworkReply::downloadProgress, this, [reply] (qint64 bytesReceived, qint64 bytesTotal) {
        qDebug().noquote() << QString("Download progress: %1/%2 bytes")
            .arg(bytesReceived)
            .arg(bytesTotal);
    });
}

void Engine::Downloader::onDownloadFinished(QNetworkReply *reply) {
    if (reply->error() != QNetworkReply::NoError) {
        emit downloadError(reply->errorString());
        reply->deleteLater();
        return;
    }

    QByteArray data = reply->readAll();
    
    if (!data.isEmpty())
        emit downloaded(data);
    else
        emit downloadError("Failed to load resource");

    reply->deleteLater();
}