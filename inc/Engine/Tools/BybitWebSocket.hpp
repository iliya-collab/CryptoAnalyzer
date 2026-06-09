#pragma once

#include <QObject>
#include <QWebSocket>
#include <QTimer>
#include <QElapsedTimer>
#include <QAtomicInt>
#include <QSet>
#include <QReadWriteLock>
#include <QThread>
#include <expected>

#include "Engine/Tools/StdTypes.hpp"

// Вынести обработку delta обнавлений в AppEngine
// Убрать m_orderBooks из BybitWebSocket

namespace Engine {
    /*
    *   Класс для работы с WebSocket на бирже Bybit
    */
    class BybitWebSocket : public QObject {
        Q_OBJECT

    private:

        // Отправка сообщения о подписке
        void sendSubscriptionMessage(const QStringList& streams);
        // Отправка сообщения об отписке
        void sendUnsubscriptionMessage(const QStringList& streams);
        // Отправка сообщения о пинге
        void sendPingMessage();
        // Обработка сообщения
        void messageReceived(const QJsonObject& obj);
        // Превращает строковое сообщение в json объект
        std::expected<QJsonObject, QString> parseTextMessage(const QString &message);
        // Обнавление тикера
        void updateTicker(const QJsonObject& json);
        // Обнавление стакана цен
        void updateOrderbook(const QJsonObject& json);
        void updateKline(const QJsonObject& json);

        // Метод для создания канала ticker
        QString createTickerStream(const QString& coin);
        // Метод для создания канала orderbook
        QString createOrderbookStream(const QString& coin);
        QString createKlineStream(const QString& coin);
        // Методы настройки веб-сокета
        void setupWebSocket();
        // Метода для настройки соединений
        void setupConnections();
        // Метод для правильного закрытия веб-сокеты
        void cleanup();
        // Метод для освобождения и закрытия
        void closeAfterFlush();

        QWebSocket* m_webSocket; // Веб-сокет
        API m_api; // API для работы с приватными каналами
        QTimer* m_pingTimer; // Таймер проверки доступности соединения (активный ping)
        QSet<QString> m_usedStreams; // Используемые каналы
        quint64 m_nextReqId = 1; // Уникальный id для подписки на каналы
        bool m_pendingClose = false;

        // Максимальное кол-во каналов в 1ой подписке
        const int MAX_STREAMS_PER_SUBSCRIPTION = 10;
        // каждые 20 с
        const int ACTIVE_PING_INTERVAL = 20000;

    public:

        explicit BybitWebSocket(QObject* parent = nullptr);
        ~BybitWebSocket();

        // Каналы для подклячения
        enum class Stream {
            Ticker,     // Тикер
            Orderbook,  // Стакан ордеров
            Kline       // Свечи
        };

        void initAPI(const API& api);

        // Открывает websocket
        void open();
        // Закрывает websocket
        void close();
        // Проверяет открыт ли websocket
        bool isOpen();
        // Формирует каналы для монеты и отправляет сообщение о подписке на них, если websocket открыт
        void subscribeToStream(const QString& coin, QSet<Stream> streams);
        // Потправляет сообщение о подписке на все используемые каналы
        void connectToStreams();
        // Потправляет сообщение об отписке на все используемые каналы
        void disconnectFromStreams();

    signals:

        void updatedTicker(const stTicker& newTicker);
        void updatedOrderbook(const stOrderBook& newOrderBook);
        void updatedKline(const stKline& newKline);

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

        void onBytesWritten(qint64 bytes);

        void onPing();

    };
    
}