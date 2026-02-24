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
    
    class BybitRestAPI : public QObject {
        Q_OBJECT
    private:

        QNetworkAccessManager* m_manager;
        QString m_baseEndpoint;
        API m_api;

        struct APIHeaders {
            QString X_BAPI_API_KEY;
            QString X_BAPI_TIMESTAMP; 
            QString X_BAPI_SIGN;
            QString X_BAPI_RECV_WINDOW;
        };
        
        
        void handleResponse();
        QString generateSignature(const QString& timestamp, const QString& recv_window, const QString& queryString = "");
        void initBaseEndpoint();
        APIHeaders initAPIHeaders(const QString& queryString = "");

        void addAPIHeaders(const QUrl& url, QNetworkRequest& request);

    public:

        static qint64 TIMEOUT;

        BybitRestAPI(const API& api, QObject* parent = nullptr);
        ~BybitRestAPI() = default;

        void requestEndpoint(const QString& endpoint, const QUrlQuery& params = QUrlQuery());

    signals:
    
        void dataReceived(const QJsonObject& obj);
        void errorOccurred(const QString &error);

    };
    
} // namespace Engine
