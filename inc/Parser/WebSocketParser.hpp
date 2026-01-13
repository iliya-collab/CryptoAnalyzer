#pragma once

#include <QObject>
#include <QWebSocket>
#include <QTimer>
#include <QMap>
#include <QSet>
#include <QReadWriteLock>
#include <QThread>

#include <memory>
#include <optional>
#include <qobject.h>

class WebSocketParser : public QObject {
    Q_OBJECT

public:

    enum TMarketData {
        SPOT,
        FUTURES
    };

    enum Channel {
        TICKER,
        BOOKS1,
        BOOKS5
    };

protected:

    // Метод для отправки сообшения о подписи на монету
    virtual void sendSubscriptionMessage(const QStringList &streams) = 0;
    // Метод для отправки сообшения об отписки на монету
    virtual void sendUnsubscriptionMessage(const QStringList &streams) = 0;

    // Метод обновления для ticker
    virtual void updateTicker(const QJsonObject &json) = 0;
    // Метод обновления для books1
    virtual void updateBooks1(const QJsonObject &json) = 0;
    // Метод обновления для books5
    virtual void updateBooks5(const QJsonObject &json) = 0;

    // Метод для подписки монеты на ticker
    virtual QString tickerStream(QString &coin) = 0;
    // Метод для подписки монеты на books1
    virtual QString books1Stream(QString &coin) = 0;
    // Метод для подписки монеты на books5
    virtual QString books5Stream(QString &coin) = 0;

    virtual std::optional<QUrl> getURLMarketData(TMarketData TMarket) = 0;

    virtual QString formatCoin(const QString& coin) = 0;

    TMarketData t_market;

    // Метод настройки веб-сокета
    void setupWebSocket();
    void connectSignals();
    // Метод для корректного закрытия веб-сокета
    void cleanup();

    // Веб-сокет
    std::unique_ptr<QWebSocket> webSocket;
    // Таймер для повторного переподключения
    std::unique_ptr<QTimer> reconnectTimer;
    // Таймер проверки доступности соединения
    std::unique_ptr<QTimer> pingTimer;

    bool autoReconnect;
    int reconnectAttempts;
    bool isConnecting;

    QReadWriteLock dataLock;

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
        double volCсy24h;           // Объем торгов в котируемой валюте за 24ч (USDT)
        double vol24h;              // Объем торгов в базовой валюте за 24ч (BTC)
    };

    struct stInfoCoin {
        stInfoPrice stPrice;        // Информация о цене
        stInfoBooks stBooks[5];     // Стакан ордеров (channel : books1 или books5)
        stInfo24hStat st24hStat;    // Статистика за 24 часа
    };

    //  subscribedCoins = { "BTC/USDT", "ETH/USDT", "ADA/USDT", ... }
    //  currentInfoAboutCoins = { {"BTCUSDT", {...}}, ... }
    QSet<QString> subscribedCoins;
    QMap<QString, stInfoCoin> currentInfoAboutCoins;
    QList<Channel> usedChannels;

    QUrl Url;
    QString nameMarket;

    const double MIN_PRICE_CHANGE = 0.000001; // 0.0001%
    const int MAX_STREAMS_PER_SUBSCRIPTION = 10;

public:

    explicit WebSocketParser(const QString& name, TMarketData TMarket, QObject* parent = nullptr);
    virtual ~WebSocketParser();

    void subscribeToCoins(const QStringList &coins);
    void unsubscribeFromCoins(const QStringList &coins);
    void unsubscribeAllCoins();
    void connectToStream();
    void disconnectFromStream();
    bool isConnected() const;

    stInfoCoin getInfoAboutCoin(const QString &coin);
    QMap<QString, stInfoCoin> getInfoAboutAllCoins();
    QStringList getSubscribedCoins();

    QString getNameMarket() {
        return nameMarket;
    }

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

    virtual void onTextMessageReceived(const QString &message) = 0;
};
