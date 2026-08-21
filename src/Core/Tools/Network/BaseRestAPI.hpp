#pragma once
#include "../StdTypes.hpp"
#include <QObject>

namespace Core::Tools
{

    class BaseRestAPI : public QObject
    {
        Q_OBJECT
    public:

        explicit BaseRestAPI(QObject *parent = nullptr) : QObject(parent) {}
        virtual ~BaseRestAPI() = default;

        // Инициализация API
        virtual void initAPI(const API& api = API()) = 0;

        // Формирование запроса
        virtual QUrl requestEndpoint(const QString& endpoint,
                                     const QUrlQuery& params = QUrlQuery(),
                                     int timeout = -1) = 0;

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