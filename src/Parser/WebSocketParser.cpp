#include "Parser/WebSocketParser.hpp"
#include <qurl.h>

WebSocketParser::WebSocketParser(const QString& name, TMarketData TMarket, QObject* parent) :
    QObject(parent),
    nameMarket(name),
    webSocket(nullptr),
    reconnectTimer(nullptr),
    pingTimer(nullptr),
    autoReconnect(true),
    reconnectAttempts(0),
    isConnecting(false),
    t_market(TMarket)
{
    setupWebSocket();
    connectSignals();
}
WebSocketParser::~WebSocketParser() {
    cleanup();
}

void WebSocketParser::setupWebSocket() {
    QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
    sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
    sslConfig.setProtocol(QSsl::TlsV1_2OrLater);

    reconnectTimer = std::make_unique<QTimer>();
    pingTimer = std::make_unique<QTimer>();
    webSocket = std::make_unique<QWebSocket>("", QWebSocketProtocol::VersionLatest);

    webSocket->setSslConfiguration(sslConfig);
    reconnectTimer->setSingleShot(true);
}

void WebSocketParser::connectSignals() {
    connect(webSocket.get(), &QWebSocket::connected, this, &WebSocketParser::onConnected);
    connect(webSocket.get(), &QWebSocket::disconnected, this, &WebSocketParser::onDisconnected);
    connect(webSocket.get(), &QWebSocket::textMessageReceived, this, &WebSocketParser::onTextMessageReceived);
    connect(webSocket.get(), &QWebSocket::errorOccurred, this, &WebSocketParser::onError);
    connect(webSocket.get(), &QWebSocket::sslErrors, this, &WebSocketParser::onSslErrors);

    connect(reconnectTimer.get(), &QTimer::timeout, this, &WebSocketParser::reconnect);

    connect(pingTimer.get(), &QTimer::timeout, this, [this]() {
        if (webSocket->state() == QAbstractSocket::ConnectedState)
            webSocket->ping();
    });
}

void WebSocketParser::cleanup() {
    autoReconnect = false;
    isConnecting = false;

    if (reconnectTimer) {
        reconnectTimer->blockSignals(true);
        if (reconnectTimer->isActive())
            reconnectTimer->stop();
        reconnectTimer->disconnect(this);
    }

    if (pingTimer) {
        pingTimer->blockSignals(true);
        if (pingTimer->isActive())
            pingTimer->stop();
        pingTimer->disconnect(this);
    }

    if (webSocket) {
        webSocket->disconnect(this);
        if (webSocket->state() != QAbstractSocket::UnconnectedState)
            webSocket->close();
    }
}

void WebSocketParser::connectToStream() {
    if (isConnecting || webSocket->state() == QAbstractSocket::ConnectedState)
        return;

    isConnecting = true;

    webSocket->open(Url);
}

void WebSocketParser::disconnectFromStream() {
    autoReconnect = false;
    isConnecting = false;

    if (reconnectTimer && reconnectTimer->isActive())
        reconnectTimer->stop();

    if (pingTimer && pingTimer->isActive())
        pingTimer->stop();

    if (webSocket && webSocket->state() != QAbstractSocket::UnconnectedState) {
        webSocket->close();
        webSocket->abort();
    }
}

void WebSocketParser::subscribeToCoins(const QStringList &coins) {
    QWriteLocker locker(&dataLock);
    for (const QString &coin : coins) {
        QString coinUpper = coin.toUpper();
        if (!subscribedCoins.contains(coinUpper))
            subscribedCoins.insert(coinUpper);
    }
    locker.unlock();

    if (isConnected())
        sendSubscriptionMessage();
}

void WebSocketParser::unsubscribeFromCoins(const QStringList &coins) {
    if (coins.isEmpty())
        return;

    QStringList streamsToUnsubscribe;
    QWriteLocker locker(&dataLock);

    for (QString coin : coins) {
        QString coinUpper = coin.toUpper();
        if (subscribedCoins.contains(coinUpper)) {
            subscribedCoins.remove(coinUpper);
            currentInfoAboutCoins.remove(coinUpper);
            streamsToUnsubscribe.append(coinToStream(coin));
        }
    }
    locker.unlock();

    if (isConnected() && !streamsToUnsubscribe.isEmpty())
        sendUnsubscriptionMessage(streamsToUnsubscribe);
}

void WebSocketParser::unsubscribeAllCoins() {
     QWriteLocker locker(&dataLock);

    if (subscribedCoins.isEmpty())
        return;

    QStringList allStreams;
    for (QString coin : subscribedCoins)
        allStreams.append(coinToStream(coin));

    subscribedCoins.clear();
    currentInfoAboutCoins.clear();

    locker.unlock();


    if (isConnected() && !allStreams.isEmpty())
        sendUnsubscriptionMessage(allStreams);
}

WebSocketParser::stInfoCoin WebSocketParser::getInfoAboutCoin(const QString &coin) {
    QReadLocker locker(&dataLock);
    return currentInfoAboutCoins[coin.toUpper()];
}

QMap<QString, WebSocketParser::stInfoCoin> WebSocketParser::getInfoAboutAllCoins() {
    QReadLocker locker(&dataLock);
    return currentInfoAboutCoins;
}

QStringList WebSocketParser::getSubscribedCoins() {
    QReadLocker locker(&dataLock);
    return subscribedCoins.values();
}

bool WebSocketParser::isConnected() const {
    return webSocket && webSocket->state() == QAbstractSocket::ConnectedState;
}

void WebSocketParser::onConnected() {
    qDebug() << nameMarket << "Connected to WebSocket";
    isConnecting = false;
    reconnectAttempts = 0;

    pingTimer->start(30000);

    if (!subscribedCoins.isEmpty())
        QTimer::singleShot(100, this, &WebSocketParser::sendSubscriptionMessage);

    emit connected();
}

void WebSocketParser::onDisconnected() {
    qDebug() << nameMarket << "Disconnected from WebSocket";
    if (pingTimer && pingTimer->isActive())
        pingTimer->stop();

    emit disconnected();

    if (autoReconnect && reconnectAttempts < 10) {
        int delay = qMin(60000, (1 << reconnectAttempts) * 1000);
        reconnectTimer->start(delay);
        reconnectAttempts++;
    }
}

void WebSocketParser::onError(QAbstractSocket::SocketError error) {
    isConnecting = false;
    QString errorString = webSocket->errorString();
    qDebug() << nameMarket << "WebSocket error:" << error << "-" << errorString;
    emit errorOccurred(errorString);
}

void WebSocketParser::onSslErrors(const QList<QSslError> &errors) {
    qDebug() << nameMarket << "SSL errors occurred:";
    for (const QSslError &error : errors)
        qDebug() << " -" << error.errorString();

    webSocket->ignoreSslErrors();
}

void WebSocketParser::reconnect() {
    if (autoReconnect && !isConnecting)
        connectToStream();
}
