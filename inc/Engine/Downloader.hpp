#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QBuffer>
#include <QUrl>

namespace Engine {

    class Downloader : public QObject {
        Q_OBJECT

    private:

        QNetworkAccessManager* m_manager;

    public:

        explicit Downloader(QObject* parent = nullptr);
        void download(const QString& urlResource);

    signals:

        void downloaded(const QByteArray& bytes);
        void downloadError(const QString& error);
    
    private slots:

        void onDownloadFinished(QNetworkReply* reply);

    };

}