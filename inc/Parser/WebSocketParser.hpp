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

class WebSocketParser : public QObject {
    Q_OBJECT

public:

    struct stInfoPrice {
        double curPrice;            // Последняя цена сделки
        double prevPrice;           // Предыдущая последняя цена сделки
        double difPrice;            // Разница между curPrice и prevPrice
    };

    struct stInfoBooks {
        double bidPrice;            // Лучшая цена покупки
        double askPrice;            // Лучшая цена продажи
        double bidSize;             // Объем по лучшей цене покупки
        double askSize;             // Объем по лучшей цене продажи 
    };

    struct stInfo24hStat {
        double high24h;             // Максимальная цена за 24 часа
        double low24h;              // Минимальная цена за 24 часа
        double volCcy24h;           // Объем торгов в котируемой валюте за 24ч (USDT)
        double vol24h;              // Объем торгов в базовой валюте за 24ч (BTC)
    };

    struct stInfoCoin {
        stInfoPrice stPrice;        // Информация о цене
        stInfoBooks stBooks[20];    // Стакан ордеров (до 20)
        stInfo24hStat st24hStat;    // Статистика за 24 часа
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
    // Метод обновления для books5
    virtual void updateBooks5(const QJsonObject &json) = 0;
    // Метод обновления для books10
    virtual void updateBooks10(const QJsonObject &json) = 0;
    // Метод обновления для books20
    virtual void updateBooks20(const QJsonObject &json) = 0;

    // Метод для подписки монеты на ticker
    virtual QString tickerStream(const QString &coin) = 0;
    // Метод для подписки монеты на books5
    virtual QString books5Stream(const QString &coin) = 0;
    // Метод для подписки монеты на books10
    virtual QString books10Stream(const QString &coin) = 0;
    // Метод для подписки монеты на books20
    virtual QString books20Stream(const QString &coin) = 0;

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
    int getChannel(const QString& channel);

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
    QMap<QString, stInfoCoin> currentInfoAboutCoins;

    TMarketData t_market;
    QUrl Url;
    QString nameMarket;
    int _Channels;

    const double MIN_PRICE_CHANGE = 0.000001;       // 0.0001%
    const int MAX_STREAMS_PER_SUBSCRIPTION = 10;
    const int ACTIVE_PING_INTERVAL = 10000;         // каждые 10 с
    const int MIN_CHANGE_TIME = 500;                // каждые 500 мс

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

    void addChannels(const QString& channels);
    void deleteChannels(const QString& channels);
    void deleteAllChannels();

    stInfoCoin getInfoAboutCoin(const QString &coin);
    QMap<QString, stInfoCoin> getInfoAboutAllCoins();
    QStringList getSubscribedCoins();

    QString getNameMarket();

signals:

    void updated(const QString& symbol, const stInfoCoin& info);
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
