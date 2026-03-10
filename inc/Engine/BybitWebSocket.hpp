#pragma once

#include <QObject>
#include <QWebSocket>
#include <QTimer>
#include <QMap>
#include <QSet>
#include <QReadWriteLock>
#include <QThread>
#include <QMetaEnum>

#include <memory>
#include <expected>

#include "Engine/StdTypes.hpp"

namespace Engine {

    class BybitWebSocket : public QObject {
        Q_OBJECT

    protected:

        // Метод для отправки сообшения о подписи на монету
        void sendSubscriptionMessage(const QStringList &streams);
        // Метод для отправки сообшения об отписки на монету
        void sendUnsubscriptionMessage(const QStringList &streams);
        // Метод для отправки сообшения
        void messageReceived(const QJsonObject &obj);
        
        // Метод для обновления канала ticker
        void updateTicker(const QJsonObject &json);
        // Метод для обновления канала order books
        void updateOrderBooks(const QJsonObject &json);
        
        // Метод для подписки монеты на канал ticker
        QString tickerStream(const QString& coin);
        // Метод для подписки монеты на канал books
        QString orderBooksStream(const QString& coin);

        // Превращает строковое сообщение в json объект
        std::expected<QJsonObject, QString> parseTextMessage(const QString &message);
        // Отправляет ping для поддержки соединения веб-сокета (реактивный ping)
        void sendPingMessage(const QJsonObject& obj);
        
        // Метод настройки веб-сокета
        void setupWebSocket();
        void setupConnections();
        void cleanup();
        void setupUrl(TMarket market);

        // Веб-сокет
        std::unique_ptr<QWebSocket> m_webSocket;
        // Таймер для повторного переподключения
        std::unique_ptr<QTimer> m_reconnectTimer;
        // Таймер проверки доступности соединения (активный ping)
        std::unique_ptr<QTimer> m_pingTimer;

        bool m_autoReconnect;
        int m_reconnectAttempts;
        bool m_isConnecting;
        bool m_isCorrectInit;

        QReadWriteLock m_dataLock;

        // Подписываемая монета
        QString m_subscribedCoin;
        // Используемые каналы
        QSet<QString> m_usedStreams;
        // Адрес для подлючения
        QUrl m_url;

        // Максимальная глубина стакана ордеров 
        const int MAX_DEPTH_BOOKS = 50;
        // Максимальное кол-во каналов в 1ой подписке
        const int MAX_STREAMS_PER_SUBSCRIPTION = 10;
        // каждые 10 с
        const int ACTIVE_PING_INTERVAL = 10000;
        // каждые 200 мс
        const int MIN_CHANGE_TIME = 200;

    public:

        explicit BybitWebSocket(TMarket market, QObject *parent = nullptr);
        virtual ~BybitWebSocket();

        void subscribeToCoin(const QString& coin);
        void unsubscribeFromCoin(const QString& coin);
        void connectToStream();
        void disconnectFromStream();
        bool isConnected() const;

    signals:

        void updatedTicker(const stTicker& _ticker);
        void updatedOrderBooks(const stOrderBooks& _orderBooks);

        void connected();
        void disconnected();
        void errorOccurred(const QString& error);

    protected slots:

        void onConnected();
        void onDisconnected();
        void onError(QAbstractSocket::SocketError error);
        void onSslErrors(const QList<QSslError> &errors);
        void reconnect();
        void onTextMessageReceived(const QString &message);

    };
}