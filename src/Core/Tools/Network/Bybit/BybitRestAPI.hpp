#pragma once
#include "Tools/Network/BaseRestAPI.hpp"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QUrlQuery>

namespace Core::Tools
{

    class BybitRestAPI : public BaseRestAPI
    {
        Q_OBJECT
    private:

        QNetworkAccessManager* m_manager;

        // Структура API заголовка
        struct ApiHeaders {
            QString X_BAPI_API_KEY;
            QString X_BAPI_TIMESTAMP; 
            QString X_BAPI_SIGN;
            QString X_BAPI_RECV_WINDOW;
        };
        
        // Генерирует сигнатуру для поля X_BAPI_SIGN
        QString generateSignature(const QString& timesTamp, const QString& recvWindow, const QString& queryString = "");
        // Инициализирует API заголовок учитывая параметры запроса
        ApiHeaders initApiHeaders(const QString& queryString = "");
        // Добавляет API заголовок к запросу
        void applyHeaders(QNetworkRequest& request, const ApiHeaders& headers);

    private slots:

        void onHandleResponse() override;

    public:

        BybitRestAPI(QObject* parent = nullptr);
        ~BybitRestAPI() = default;

        void init(const Api& api = Api()) override;

        QUrl requestEndpointGet(const QString& endpoint, const QUrlQuery& params = QUrlQuery(), int timeout = -1) override;
        QUrl requestEndpointPost(const QString& endpoint, const QByteArray& jsonBody = QByteArray(), int timeout = -1) override;

    };
    
}
