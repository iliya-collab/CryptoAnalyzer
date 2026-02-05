#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrlQuery>

#include <QJsonDocument>
#include <QJsonObject>

#include <expected>

//https://chat.deepseek.com/share/9a3hp7aq92yfyt61wa

namespace Engine {
    
    class BybitRestAPI : public QObject {
        Q_OBJECT
    private:

        struct APIHeaders {
            QString X_BAPI_API_KEY;
            QString X_BAPI_TIMESTAMP; 
            QString X_BAPI_SIGN;
            QString X_BAPI_RECV_WINDOW;
        };

        QNetworkAccessManager* m_manager;

        std::expected<QJsonObject, QString> m_reply;

        QString m_api_key;
        QString m_secret_key;
        bool m_testnet;

        QString m_baseEndpoint;
        
        QString generateSignature(const QString& timestamp, const QString& recv_window, const QString& params = "");
        void initBaseEndpoint();
        APIHeaders initAPIHeaders(const QString& params = "");

        std::expected<QJsonObject, QString> onRequestFinished(QNetworkReply* reply);

    public:

        BybitRestAPI(const QString& apiKey, const QString& secretKey, bool testNet, QObject* parent = nullptr);
        ~BybitRestAPI() = default;

        void requestEndpoint(const QString& endpoint, const QString& params = "");

        std::expected<QJsonObject, QString> getReply();
        
    };
    
} // namespace Engine
