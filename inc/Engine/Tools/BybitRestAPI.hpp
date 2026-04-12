#pragma once

#include "Engine/StdTypes.hpp"

#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QUrlQuery>

namespace Engine {
    /*
    *   Класс для создания запросов к бирже ByBit
    */
    class BybitRestAPI : public QObject {
        Q_OBJECT
    private:

        QNetworkAccessManager* m_manager;
        QString m_baseEndpoint;
        QString m_apiKey;
        QString m_secretKey;

        struct APIHeaders {
            QString X_BAPI_API_KEY;
            QString X_BAPI_TIMESTAMP; 
            QString X_BAPI_SIGN;
            QString X_BAPI_RECV_WINDOW;
        };
        
        void handleResponse();
        QString generateSignature(const QString& timestamp, const QString& recv_window, const QString& queryString = "");
        void initBaseEndpoint(bool is_testnet);
        APIHeaders initAPIHeaders(const QString& queryString = "");

        void addAPIHeaders(const QUrl& url, QNetworkRequest& request);

    public:

        BybitRestAPI(const QString& api_key, const QString& secret_key, bool is_testnet, QObject* parent = nullptr);
        ~BybitRestAPI() = default;

        void requestEndpoint(const QString& endpoint, const QUrlQuery& params = QUrlQuery(), int timeout = -1);

    signals:
    
        void dataReceived(const QJsonObject& obj);
        void errorOccurred(const QString &error);
        void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

    };
    
} // namespace Engine
