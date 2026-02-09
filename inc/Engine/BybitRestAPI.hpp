#pragma once

#include "Engine/IRestAPI.hpp"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>


namespace Engine {
    
    class BybitRestAPI : public IRestAPI {
        Q_OBJECT
    private:

        struct APIHeaders {
            QString X_BAPI_API_KEY;
            QString X_BAPI_TIMESTAMP; 
            QString X_BAPI_SIGN;
            QString X_BAPI_RECV_WINDOW;
        };

        API m_api;

        QString generateSignature(const QString& timestamp, const QString& recv_window, const QString& queryString = "");
        void initBaseEndpoint();
        APIHeaders initAPIHeaders(const QString& queryString = "");

    public:

        BybitRestAPI(const API& api, QObject* parent = nullptr);
        ~BybitRestAPI() = default;

        void requestEndpoint(const QString& endpoint, const QUrlQuery& params = QUrlQuery(), int timeout = -1) override;

    };
    
} // namespace Engine
