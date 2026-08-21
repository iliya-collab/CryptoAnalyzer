#pragma once
#include "Tools/StdTypes.hpp"
#include <QObject>
#include <QWebSocket>
#include <QTimer>
#include <QElapsedTimer>
#include <QAtomicInt>
#include <QSet>
#include <QReadWriteLock>
#include <QThread>
#include <expected>

namespace Core::Tools {

    class BybitWebSocket : public QObject {
        Q_OBJECT

    public:

        // Тип сокета: публичный (рыночные данные) или приватный (данные аккаунта)
        enum class SocketType {
            Public,
            Private
        };

        // Каналы для подклячения
        enum class Stream {
            Ticker,             // Публичный: Тикер
            Orderbook,          // Публичный: Стакан ордеров
            Kline,              // Публичный: Свечи
            PublicTrade         // Публичный: Трейд
        };

        explicit BybitWebSocket(SocketType type, QObject* parent = nullptr);
        ~BybitWebSocket();

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

        double getLastPing() { return m_lastPingMs; };
        SocketType getType() const { return m_type; }

    signals:

        // Сигналы для обновления публичных каналов
        void updatedTicker(const Core::Tools::Ticker& newTicker);
        void updatedOrderbook(const Core::Tools::Orderbook& newOrderBook);
        void updatedKline(const Core::Tools::Kline& newKline);
        void updatedPublicTrade(const Core::Tools::PublicTrades& newPublicTrades);

        void pingMeasured(double lastPing);

        // Системные сигналы сокета
        void connected(); // Испускается, когда websocket успешно открылся
        void disconnected(); // Испускается, когда websocket закрылся
        void errorOccurred(const QString& error); // Испускается, когда появилась ошибка или при подключении websocket, или при ssl ошибках
        void authenticated(); // Успешная аутентификация
        void authenticationError(const QString& errorMessage); // Ошибка аутентификации

    private slots:

        void onConnected(); // Обработка подключения
        void onDisconnected(); // Обработка отключения
        void onError(QAbstractSocket::SocketError error); // Обработка ошибок при подключении
        void onSslErrors(const QList<QSslError>& errors); // Обработка ssl ошибок
        void onTextMessageReceived(const QString& message); // Обработка принятого сообщения
        void onBytesWritten(qint64 bytes);
        void onPing(); // Вызывается по таймеру m_pingTimer

    private:

        // Внутренние методы отправки запросов
        void sendSubscriptionMessage(const QStringList& streams); // Отправка сообщения о подписке
        void sendUnsubscriptionMessage(const QStringList& streams); // Отправка сообщения об отписке
        void sendAuthMessage(); // Отправка сообщения об авторизации
        void sendPingMessage(); // Отправка сообщения о пинге

        // Криптография для приватного канала
        QString generateSignature(const QString& apiKey, const QString& apiSecret, const QString& expires);

        // Обработка входящих данных
        void messageReceived(const QJsonObject& obj); // Обработка сообщения
        std::expected<QJsonObject, QString> parseTextMessage(const QString &message); // Превращает строковое сообщение в json объект

        // Обновление публичных данных
        void updateTicker(const QJsonObject& json);
        void updateOrderbook(const QJsonObject& json);
        void updateKline(const QJsonObject& json);
        void updatePublicTrade(const QJsonObject& json);

        // Генерация строк топиков для Bybit
        QString createTickerStream(const QString& coin);
        QString createOrderbookStream(const QString& coin);
        QString createKlineStream(const QString& coin);
        QString createPublicTradeStream(const QString& coin);

        // Настройка и очистка
        void setupWebSocket(); // Методы настройки веб-сокета
        void setupConnections(); // Метода для настройки соединений
        void cleanup(); // Метод для правильного закрытия веб-сокеты
        void closeAfterFlush(); // Метод для освобождения и закрытия
        void cleanupPingTimestamps();

        // Переменные состояния
        SocketType m_type;           // Тип сокета
        API m_api;                   // API
        QWebSocket* m_webSocket;     // Веб-сокет
        QTimer* m_pingTimer;         // Таймер для отправки ping сообщений
        QSet<QString> m_usedStreams; // Список активных подписок (например, "order", "tickers.BTCUSDT")
        quint64 m_nextReqId = 1;     // Счетчик ID для запросов
        bool m_pendingClose = false;

        // Измерение пинга
        QMap<QString, qint64> m_pingTimestamps; // req_id -> timestamp отправки
        double m_lastPingMs = 0;

        // Константы ограничений Bybit
        const int MAX_STREAMS_PER_SUBSCRIPTION = 10;
        const int ACTIVE_PING_INTERVAL = 20000; // 20 секунд
    };
    
}