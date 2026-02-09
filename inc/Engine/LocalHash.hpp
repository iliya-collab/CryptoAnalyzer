#pragma once

#include "Engine/Downloader.hpp"

#include <QFile>
#include <QDir>

namespace Engine {

    class LocalHash : public QObject {
        Q_OBJECT
    private:

        QStringList pendingDownloads;
        int currentIndex = 0;
        QHash<QString, QByteArray> completedDownloads;

        void startNextDownload();

        void singleDownload(const QString& url, const QString& filePath);
        
        QString generateHash(const QString& url);
        
    public:
        
        explicit LocalHash(QObject* parent = nullptr) : QObject(parent) {}

        void download(const QStringList& urls);

    signals:

        void hashError(const QString& error);
        void allHashReady(const QHash<QString, QByteArray>& allBytes);

    };

}