#pragma once

#include <QObject>
#include <QWebSocket>
#include <QTimer>
#include <QSet>
#include <QReadWriteLock>
#include <QThread>

#include <expected>

#include "Engine/StdTypes.hpp"

namespace Engine {

    /*
    *   Класс для работы с WebSocket на бирже Bybit
    */
    class BybitWebSocket : public QObject {
        Q_OBJECT

    protected:

        // Методы для отправки сообшений
        // Сообщение о подписке
        void sendSubscriptionMessage(const QStringList& streams);
        // Сообщение о отписке
        void sendUnsubscriptionMessage(const QStringList& streams);
        // Отправка сообщения о пинге
        void sendPingMessage(const QJsonObject& obj);
        // Обработка сообщения
        void messageReceived(const QJsonObject& obj);
        
        // Превращает строковое сообщение в json объект
        std::expected<QJsonObject, QString> parseTextMessage(const QString &message);

        // Методы для обновления каналов
        // Обнавление тикера
        void updateTicker(const QJsonObject& json);
        // Обнавление стакана цен
        void updateOrderbook(const QJsonObject& json);

        // Метод для создания каналов
        QString createTickerStream(const QString& coin);
        QString createOrderbookStream(const QString& coin);
        
        // Методы настройки веб-сокета
        void setupWebSocket();
        void setupConnections();
        // Метод для правильного закрытия веб-сокеты
        void cleanup();

        // Веб-сокет
        QWebSocket* m_webSocket;
        // Таймер проверки доступности соединения (активный ping)
        QTimer* m_pingTimer;

        QReadWriteLock m_dataLock;

        // Используемые каналы
        QSet<QString> m_usedStreams;

        // Максимальное кол-во каналов в 1ой подписке
        const int MAX_STREAMS_PER_SUBSCRIPTION = 10;
        // каждые 10 с
        const int ACTIVE_PING_INTERVAL = 10000;

    public:

        explicit BybitWebSocket(QObject* parent = nullptr);
        ~BybitWebSocket();

        enum class Stream {
            Ticker,
            Orderbook,
            Kline
        };

        // Открывает websocket по указанному адресу
        void open(const QUrl& baseEndpont);
        // Закрывает websocket
        void close();
        // Проверяет открыт ли websocket
        bool isOpen();
        // Подписывает монету на каналы
        void subscribeToStream(const QString& coin, QSet<Stream> streams);
        // Подключается ко всем каналам
        void connectToStream();
        // Отключается от всех каналов
        void disconnectFromStream();

    signals:

        void updatedTicker(const stTicker& ticker);
        void updatedOrderbook(const stOrderBooks& orderBooks);

        // Испускается, когда websocket успешно открылся
        void connected();
        // Испускается, когда websocket закрылся
        void disconnected();
        // Испускается, когда появилась ошибка или при подключении websocket, или при ssl ошибках
        void errorOccurred(const QString& error);

    private slots:

        // Обработка подключения
        void onConnected();
        // Обработка отключения
        void onDisconnected();
        // Обработка ошибок при подключении
        void onError(QAbstractSocket::SocketError error);
        // Обработка ssl ошибок
        void onSslErrors(const QList<QSslError>& errors);
        // Обработка принятого сообщения
        void onTextMessageReceived(const QString& message);

    };
    
}