#pragma once
#include "Tools/StdTypes.hpp"
#include <QObject>
#include <QWebSocket>

namespace Core::Tools
{

    class BaseWebSocket : public QObject
    {
        Q_OBJECT
    public:

        // Тип сокета: публичный (рыночные данные) или приватный (данные аккаунта)
        enum class SocketType {
            Public,
            Private
        };

        explicit BaseWebSocket(SocketType type = SocketType::Public, QObject* parent = nullptr);
        virtual ~BaseWebSocket();

        virtual void initApi(const Api& api) = 0;

        // Открыть websocket
        void open();
        // Закрыть websocket
        void close();
        // Принудительный перезапуск соединения
        void reconnect();
        // Проверить открыт ли websocket
        bool isOpen();
        // Отправка сообщения
        void sendMessage(const QString& message);

    protected:

        void setupWebSocket(); // Настройка веб-сокета
        void setupConnections(); // Настройка соединений

        virtual void sendAuthMessage() = 0; // Отправка сообщения об авторизации
        virtual void sendPingMessage() = 0; // Отправка сообщения о пинге

        SocketType m_type;                          // Тип сокета
        std::unique_ptr<QWebSocket> m_webSocket;    // Веб-сокет
        std::unique_ptr<QTimer> m_pingTimer;        // Таймер для отправки ping сообщений
        std::unique_ptr<QTimer> m_reconnectTimer;   // Таймер для повторных попыток
        bool m_isManualClose = false;               // Закрыли мы сокет сами или произошел сбой
        int m_reconnectDelay = 1000;                // Задержка между попытками (мс)
        QString m_connectUrl = "";

    signals:

        // Системные сигналы сокета
        void connected(); // Испускается, когда websocket успешно открылся
        void disconnected(); // Испускается, когда websocket закрылся
        void errorOccurred(const QString& error); // Испускается, когда появилась ошибка или при подключении websocket, или при ssl ошибках
        void authenticated(); // Успешная аутентификация
        void pingMeasured(double ping);
        void messageReceived(const QJsonObject& message);

    protected slots:

        void attemptReconnect(); // Слот для выполнения попытки подключения

        virtual void onConnected() = 0; // Обработка подключения
        virtual void onDisconnected() = 0; // Обработка отключения
        virtual void onError(QAbstractSocket::SocketError error) = 0; // Обработка ошибок при подключении
        virtual void onSslErrors(const QList<QSslError>& errors) = 0; // Обработка ssl ошибок
        virtual void onTextMessageReceived(const QString& message) = 0; // Обработка принятого сообщения
        virtual void onBytesWritten(qint64 bytes) = 0;
        virtual void onPing() = 0; // Вызывается по таймеру m_pingTimer

    };

}