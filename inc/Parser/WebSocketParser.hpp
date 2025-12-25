#pragma once

#include <QObject>
#include <QWebSocket>
#include <QTimer>
#include <QMap>
#include <QSet>
#include <QReadWriteLock>
#include <QThread>

#include <memory>

class WebSocketParser : public QObject {
    Q_OBJECT

protected:

    // Метод для отправки сообшения о подписи на монету
    virtual void sendSubscriptionMessage() = 0;
    // Метод для отправки сообшения об отписки на монету
    virtual void sendUnsubscriptionMessage(const QStringList &streams) = 0;
    // Метод обновления цены монеты
    virtual void processPriceUpdate(const QJsonObject &json) = 0;
    // Метод для создания потока на подписанную монету
    virtual QString coinToStream(const QString &coin) = 0;

    // Метод настройки веб-сокета
    void setupWebSocket();
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

    static QSet<QString> subscribedCoins;
    static QReadWriteLock dataLock;

    struct stInfoCoin {
        double value;       // Текущая цена
        double dif;         // Разница между новой и старой
    };

    static QMap<QString, stInfoCoin> currentInfoAboutCoins;

public:

    explicit WebSocketParser(QObject* parent = nullptr);
    virtual ~WebSocketParser();

    void subscribeToCoins(const QStringList &coins);
    void unsubscribeFromCoins(const QStringList &coins);
    void unsubscribeAllCoins();
    void connectToStream(QUrl url);
    void disconnectFromStream();
    bool isConnected() const;

    static QUrl TradeUrl;

    static stInfoCoin getInfoAboutCoin(const QString &coin);
    static QMap<QString, stInfoCoin> getInfoAboutAllCoins();
    static QStringList getSubscribedCoins();

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
    
    virtual void onTextMessageReceived(const QString &message);
};