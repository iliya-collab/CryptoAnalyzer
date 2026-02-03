#pragma once

#include <QObject>
#include <QWebSocket>
#include <QTimer>
#include <QMap>
#include <QSet>
#include <QReadWriteLock>
#include <QThread>
#include <QMetaEnum>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <memory>
#include <expected>

#include "Engine/StdTypes.hpp"

namespace Engine {

    class WebSocket : public QObject {
        Q_OBJECT
    protected:

        // Метод для отправки сообшения о подписи на монету
        virtual void sendSubscriptionMessage(const QStringList &streams) = 0;
        // Метод для отправки сообшения об отписки на монету
        virtual void sendUnsubscriptionMessage(const QStringList &streams) = 0;
        // Метод для отправки сообшения
        virtual void messageReceived(const QJsonObject &obj) = 0;

        // Метод для обновления канала ticker
        virtual void updateTicker(const QJsonObject &json) = 0;
        // Метод для обновления канала order books
        virtual void updateOrderBooks(const QJsonObject &json) = 0;

        // Метод для подписки монеты на канал ticker
        virtual QString tickerStream(const QString &coin) = 0;
        // Метод для подписки монеты на канал books
        virtual QString orderBooksStream(const QString &coin) = 0;

        // Превращает строковое сообщение в json объект
        std::expected<QJsonObject, QString> parseTextMessage(const QString &message);
        // Отправляет ping для поддержки соединения веб-сокета (реактивный ping)
        void sendPingMessage(const QJsonObject& obj);
        
        // Метод настройки веб-сокета
        void setupWebSocket();
        void setupConnections();
        void cleanup();

        // Веб-сокет
        std::unique_ptr<QWebSocket> webSocket;
        // Таймер для повторного переподключения
        std::unique_ptr<QTimer> reconnectTimer;
        // Таймер проверки доступности соединения (активный ping)
        std::unique_ptr<QTimer> pingTimer;

        bool autoReconnect;
        int reconnectAttempts;
        bool isConnecting;
        bool isCorrectInit;

        QReadWriteLock dataLock;

        QSet<QString> subscribedCoins;
        QSet<QString> usedStreams;

        // Адрес для подлючения
        QUrl Url;
        // Уникальный строковый индентификатор
        QString uniqueId;

        const int MAX_DEPTH_BOOKS = 50;                 // Максимальная глубина стакана ордеров 
        const int MAX_STREAMS_PER_SUBSCRIPTION = 10;    // Максимальное число подписок/отписок в одном сообщении
        const int ACTIVE_PING_INTERVAL = 10000;         // каждые 10 с
        const int MIN_CHANGE_TIME = 200;                // каждые 200 мс

    public:

        explicit WebSocket(const QString& id, QObject* parent = nullptr);
        virtual ~WebSocket();

        void subscribeToCoins(const QStringList &coins);
        void unsubscribeFromCoins(const QStringList &coins);
        void unsubscribeAllCoins();
        void connectToStream();
        void disconnectFromStream();
        bool isConnected() const;

        QStringList getSubscribedCoins();

    signals:

        void updatedTicker(const stTicker& _ticker);
        void updatedOrderBooks(const stOrderBooks& _orderBooks);

        void connected();
        void disconnected();
        void errorOccurred(const QString &error);

    protected slots:

        void onConnected();
        void onDisconnected();
        void onError(QAbstractSocket::SocketError error);
        void onSslErrors(const QList<QSslError> &errors);
        void reconnect();
        void onTextMessageReceived(const QString &message);
    };
}
