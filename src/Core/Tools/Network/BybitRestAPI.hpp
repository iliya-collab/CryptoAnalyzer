#pragma once

#include "../StdTypes.hpp"
#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QUrlQuery>

namespace Core::Tools {
    /*
    *   Класс для создания запросов к бирже ByBit
    */
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
        
        // Обработка ответа
        void handleResponse();
        // Генерирует сигнатуру для поля X_BAPI_SIGN
        QString generateSignature(const QString& timesTamp, const QString& recvWindow, const QString& queryString = "");
        // Инициализирует API заголовок учитывая параметры запроса
        APIHeaders initAPIHeaders(const QString& queryString = "");
        // Добавляет API заголовок к запросу
        void addAPIHeaders(const QUrl& url, QNetworkRequest& request);

    public:

        BybitRestAPI(QObject* parent = nullptr);
        ~BybitRestAPI() = default;

        void initAPI(const API& api = API());

        // Формирует запрос
        // endpoint - отправляемый запрос
        // params - параметры к запросу
        // timeout - ограничение по времени на обработку запроса в мс
        void requestEndpoint(const QString& endpoint, const QUrlQuery& params = QUrlQuery(), int timeout = -1);

    signals:
    
        void dataReceived(const QJsonObject& obj);
        void errorOccurred(const QString &error);
        void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

    };
    
} // namespace Engine
