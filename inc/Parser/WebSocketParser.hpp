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

protected:

    // Метод для отправки сообшения о подписи на монету
    virtual void sendSubscriptionMessage() = 0;
    // Метод для отправки сообшения об отписки на монету
    virtual void sendUnsubscriptionMessage(const QStringList &streams) = 0;
    // Метод обновления цены монеты
    virtual void processPriceUpdate(const QJsonObject &json) = 0;
    // Метод для создания потока на подписанную монету
    virtual QString coinToStream(QString &coin) = 0;

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

    struct stInfoCoin {
        double value;       // Текущая цена
        double dif;         // Разница между новой и старой
    };

    //  subscribedCoins = { "BTC/USDT", "ETH/USDT", "ADA/USDT" }
    //  currentInfoAboutCoins = { {"BTCUSDT", {...}}, ... }
    QSet<QString> subscribedCoins;
    QMap<QString, stInfoCoin> currentInfoAboutCoins;

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

    void priceUpdated(const QString &symbol, double price);
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
