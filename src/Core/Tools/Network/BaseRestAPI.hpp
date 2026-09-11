#pragma once
#include "Tools/StdTypes.hpp"
#include <QObject>

namespace Core::Tools
{

    class BaseRestAPI : public QObject
    {
        Q_OBJECT
    public:

        explicit BaseRestAPI(QObject *parent = nullptr) : QObject(parent) {}
        virtual ~BaseRestAPI() = default;

        void setApi(const Api& api = Api()) { m_api = api; }

        void setUrl(const QString& url) { m_baseEndpoint = url; }

        virtual void init(const Api& api = Api()) = 0;

        // Формирование GET запроса
        virtual QUrl requestEndpointGet(const QString& endpoint, const QUrlQuery& params = QUrlQuery(), int timeout = -1) = 0;
        // Формирование POST запроса
        virtual QUrl requestEndpointPost(const QString& endpoint, const QByteArray& jsonBody = QByteArray(), int timeout = -1) = 0;

    protected:

        Api m_api{};
        QString m_baseEndpoint = "";

    protected slots:

        // Обработка ответа
        virtual void onHandleResponse() = 0;

    signals:

        // Данные пришли
        void dataReceived(const QUrl& url, const QByteArray& data);
        // Возникла ошибка
        void errorOccurred(const QString &error);
        // Отслеживание процесса загрузки
        void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

    };

}