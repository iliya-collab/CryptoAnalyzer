#pragma once

#include <QObject>
#include <QWebSocket>
#include <QTimer>
#include <QSet>
#include <QReadWriteLock>
#include <QThread>

#include <memory>
#include <expected>

#include "Engine/StdTypes.hpp"

namespace Engine {

    class BybitWebSocket : public QObject {
        Q_OBJECT

    protected:

        // Методы для отправки сообшений
        void sendSubscriptionMessage(const QStringList& streams);
        void sendUnsubscriptionMessage(const QStringList& streams);
        void messageReceived(const QJsonObject& obj);
        void sendPingMessage(const QJsonObject& obj);
        
        // Превращает строковое сообщение в json объект
        std::expected<QJsonObject, QString> parseTextMessage(const QString &message);

        // Методы для обновления каналов
        void updateTicker(const QJsonObject& json);
        void updateOrderbook(const QJsonObject& json);

        // Метод для создания каналов
        QString createTickerStream(const QString& coin);
        QString createOrderbookStream(const QString& coin);
        
        // Методы настройки веб-сокета
        void setupWebSocket();
        void setupConnections();
        void setupUrl(TMarket market);
        // Метод для правильного закрытия веб-сокеты
        void cleanup();

        // Веб-сокет
        QWebSocket* m_webSocket;
        // Таймер для повторного переподключения
        QTimer* m_reconnectTimer;
        // Таймер проверки доступности соединения (активный ping)
        QTimer* m_pingTimer;

        bool m_autoReconnect;
        int m_reconnectAttempts;
        bool m_isConnecting;
        bool m_isCorrectInit;

        QReadWriteLock m_dataLock;

        // Используемые каналы
        QSet<QString> m_usedStreams;
        // Адрес для подлючения
        QUrl m_url;

        // Максимальное кол-во каналов в 1ой подписке
        const int MAX_STREAMS_PER_SUBSCRIPTION = 10;
        // каждые 10 с
        const int ACTIVE_PING_INTERVAL = 10000;

    public:

        explicit BybitWebSocket(TMarket market, QObject* parent = nullptr);
        ~BybitWebSocket();

        enum class Stream {
            Ticker,
            Orderbook,
            Kline
        };

        void subscribeToStream(const QString& coin, Stream stream);

        void connectToStream();
        void disconnectFromStream();
        bool isConnected() const;

    signals:

        void updatedTicker(const stTicker& ticker);
        void updatedOrderbook(const stOrderBooks& orderBooks);

        void connected();
        void disconnected();
        void errorOccurred(const QString& error);

    protected slots:

        void onConnected();
        void onDisconnected();
        void onError(QAbstractSocket::SocketError error);
        void onSslErrors(const QList<QSslError>& errors);
        void reconnect();
        void onTextMessageReceived(const QString& message);

    };
}