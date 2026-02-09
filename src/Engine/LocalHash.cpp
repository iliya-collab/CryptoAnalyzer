#include "Engine/LocalHash.hpp"

#include <QCryptographicHash>
#include <QTimer>

QString Engine::LocalHash::generateHash(const QString& url) {
    return QString(QCryptographicHash::hash(url.toUtf8(), QCryptographicHash::Md5).toHex());
}

void Engine::LocalHash::singleDownload(const QString& url, const QString& filePath) {
    Downloader* downloader = new Downloader(this);

    connect(downloader, &Engine::Downloader::downloadError, this, [this, downloader, url] (const QString& error) {
        emit hashError(QString("Failed to download : %1").arg(error));
        completedDownloads.insert(url, QByteArray());
        downloader->deleteLater();
        QTimer::singleShot(0, this, &LocalHash::startNextDownload);
    });

    connect(downloader, &Engine::Downloader::downloaded, this, [this, downloader, url, filePath] (const QByteArray& bytes) {
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(bytes);
            file.close();
        } 
        completedDownloads.insert(url, bytes);
        downloader->deleteLater();
        QTimer::singleShot(0, this, &LocalHash::startNextDownload);
    });

    downloader->download(url);
}

void Engine::LocalHash::startNextDownload() {
    currentIndex++;
    
    if (currentIndex >= pendingDownloads.size()) {
        emit allHashReady(completedDownloads);
        return;
    }
    
    QString url = pendingDownloads[currentIndex];
    QString hash = generateHash(url);
    
    const QString path = QString("%1/static/%2").arg(QDir::currentPath()).arg(hash);
    
    QFile localFile(path);
    if (localFile.exists() && localFile.size() > 0) {
        if (localFile.open(QIODevice::ReadOnly)) {
            completedDownloads.insert(url, localFile.readAll());
            localFile.close();
            QTimer::singleShot(10, this, &LocalHash::startNextDownload);
        } 
    } 
    else
        singleDownload(url, path);

}

void Engine::LocalHash::download(const QStringList& urls) {
    if (urls.isEmpty()) {
        emit allHashReady(QHash<QString, QByteArray>());
        return;
    }

    pendingDownloads = urls;
    completedDownloads.clear();
    currentIndex = -1;

    startNextDownload();
}