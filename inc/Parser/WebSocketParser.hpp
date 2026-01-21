#pragma once

#include <QObject>
#include <QWebSocket>
#include <QTimer>
#include <QMap>
#include <QSet>
#include <QReadWriteLock>
#include <QThread>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <memory>
#include <expected>

// https://chat.deepseek.com/share/t1yakp1a6ukuhjk3r9

class WebSocketParser : public QObject {
    Q_OBJECT
public:

    struct Ask {
        double askPrice;                // Цена продажи
        double askSize;                 // Объем продажи 
    };

    struct Bid {
        double bidPrice;                // Цена покупки
        double bidSize;                 // Объем покупки
    };

    struct stOrderBooks {
        QString namePair;               // Название монетной пары (Binance/spot:BTCUSDT)

        QList<Bid> bids;                // Покупки
        QList<Ask> asks;                // Продажи

        double spread;                  // Спред

        double totalBidVolume;          // Общий обЪем покупки
        double totalAskVolume;          // Общий обЪем продажи
    };

    struct stTicker {
        QString namePair;               // Название монетной пары (BTCUSDT)

        double curPrice;                // Последняя цена сделки

        double high24h;                 // Максимальная цена за 24 часа
        double low24h;                  // Минимальная цена за 24 часа
        double volCcy24h;               // Объем торгов в котируемой валюте за 24ч (USDT)
        double vol24h;                  // Объем торгов в базовой валюте за 24ч (BTC)

        Ask bestAsk;                    // Лучшая продажа
        Bid bestBid;                    // Лучшая покупка
        double spread;                  // Спред
    };


protected:

    enum TMarketData {
        NONEMARKET,
        SPOT,
        FUTURES
    };

    enum Channel {
        NONECHANNEL,
        TICKER = 1,
        BOOKS5 = 2,
        BOOKS10 = 4,
        BOOKS20 = 8
    };

    // Метод для отправки сообшения о подписи на монету
    virtual void sendSubscriptionMessage(const QStringList &streams) = 0;
    // Метод для отправки сообшения об отписки на монету
    virtual void sendUnsubscriptionMessage(const QStringList &streams) = 0;
    // Метод для отправки сообшения
    virtual void messageReceived(const QJsonObject &obj) = 0;

    // Метод обновления для ticker
    virtual void updateTicker(const QJsonObject &json) = 0;
    // Метод обновления для books
    virtual void updateOrderBooks(const QJsonObject &json) = 0;

    // Метод для подписки монеты на ticker
    virtual QString tickerStream(const QString &coin) = 0;
    // Метод для подписки монеты на books
    virtual QString orderBooksStream(const QString &coin) = 0;

    // Получает нужный url для конкретного рынка сбыта (spot/futures)
    virtual std::expected<QUrl, QString> getURLMarketData() = 0;

    // Преобразует пару в запись без разделителя
    // "BTC/USDT" "BTC-USDT" "BTC-USDT-SWAP" -> "BTCUSDT"
    virtual QString formatCoin(const QString& coin) = 0;

    // Превращает строковое сообщение в json объект
    std::expected<QJsonObject, QString> parseTextMessage(const QString &message);
    // Отправляет ping для поддержки соединения веб-сокета (реактивный ping)
    void sendPingMessage(const QJsonObject& obj);
    
    // Извлекает из имени WebSocketParser рынок сбыта (Binance/spot, Binance/futures ...)
    TMarketData getTypeMarket(const QString& name);
    // Устанавливает url адресс для веб-сокета
    void WebSocketUrl();
    // Метод настройки веб-сокета
    void setupWebSocket();
    // Подлючение сигналов к веб-сокету
    void connectSignals();
    // Метод для корректного закрытия веб-сокета
    void cleanup();
    // Превращает строковое представление канала в его числовой код
    int convertChannel(const QString& channel);

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

    //  subscribedCoins = { "BTC/USDT", "ETH/USDT", "ADA/USDT", ... }
    //  currentInfoAboutCoins = { {"BTCUSDT", {...}}, ... }
    QSet<QString> subscribedCoins;
    QSet<QString> usedStreams;

    TMarketData t_market;
    QUrl Url;
    QString nameMarket;
    int _Channels = 0;

    int mxDepthBooks = 0;                           // Максимальная глубина стакана ордеров 

    const int MAX_STREAMS_PER_SUBSCRIPTION = 10;    // Максимальное число подписок/отписок в одном сообщении
    const int ACTIVE_PING_INTERVAL = 10000;         // каждые 10 с
    const int MIN_CHANGE_TIME = 200;                // каждые 200 мс

public:

    explicit WebSocketParser(const QString& name, QObject* parent = nullptr);
    virtual ~WebSocketParser();

    bool init();

    void subscribeToCoins(const QStringList &coins);
    void unsubscribeFromCoins(const QStringList &coins);
    void unsubscribeAllCoins();
    void connectToStream();
    void disconnectFromStream();
    bool isConnected() const;

    int getChannels() const;
    void addChannels(const QString& channels);
    void deleteChannels(const QString& channels);
    void deleteAllChannels();

    QStringList getSubscribedCoins();

    QString getNameMarket();

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
