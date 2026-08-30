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
        QString m_baseEndpoint;
        Api m_api;

        // Структура API заголовка
        struct APIHeaders {
            QString X_BAPI_API_KEY;
            QString X_BAPI_TIMESTAMP; 
            QString X_BAPI_SIGN;
            QString X_BAPI_RECV_WINDOW;
        };
        
        // Генерирует сигнатуру для поля X_BAPI_SIGN
        QString generateSignature(const QString& timesTamp, const QString& recvWindow, const QString& queryString = "");
        // Инициализирует API заголовок учитывая параметры запроса
        APIHeaders initAPIHeaders(const QString& queryString = "");
        // Добавляет API заголовок к запросу
        void addAPIHeaders(const QUrl& url, QNetworkRequest& request);

    private slots:

        void onHandleResponse() override;

    public:

        BybitRestAPI(QObject* parent = nullptr);
        ~BybitRestAPI() = default;

        void initApi(const Api& api = Api()) override;

        QUrl requestEndpoint(const QString& endpoint, const QUrlQuery& params = QUrlQuery(), int timeout = -1) override;

    };
    
}
