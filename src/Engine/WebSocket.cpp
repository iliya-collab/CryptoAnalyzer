#include "Engine/WebSocket.hpp"

Engine::WebSocket::WebSocket(const QString& id, QObject* parent) : QObject(parent), webSocket(nullptr), reconnectTimer(nullptr), pingTimer(nullptr) {
    autoReconnect = true; 
    isConnecting = false; 
    isCorrectInit = true; 
    reconnectAttempts = 0; 
    uniqueId = id;

    setupWebSocket();
    setupConnections();

    pingTimer->start(ACTIVE_PING_INTERVAL);
}

Engine::WebSocket::~WebSocket() {
    cleanup();
}

void Engine::WebSocket::setupWebSocket() {
    QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
    sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
    sslConfig.setProtocol(QSsl::TlsV1_2OrLater);

    reconnectTimer = std::make_unique<QTimer>();
    pingTimer = std::make_unique<QTimer>();
    webSocket = std::make_unique<QWebSocket>("", QWebSocketProtocol::VersionLatest);

    webSocket->setSslConfiguration(sslConfig);
    reconnectTimer->setSingleShot(true);
}

void Engine::WebSocket::setupConnections() {
    connect(webSocket.get(), &QWebSocket::connected, this, &Engine::WebSocket::onConnected);
    connect(webSocket.get(), &QWebSocket::disconnected, this, &Engine::WebSocket::onDisconnected);
    connect(webSocket.get(), &QWebSocket::textMessageReceived, this, &Engine::WebSocket::onTextMessageReceived);
    connect(webSocket.get(), &QWebSocket::errorOccurred, this, &Engine::WebSocket::onError);
    connect(webSocket.get(), &QWebSocket::sslErrors, this, &Engine::WebSocket::onSslErrors);

    connect(reconnectTimer.get(), &QTimer::timeout, this, &Engine::WebSocket::reconnect);

    connect(pingTimer.get(), &QTimer::timeout, this, [this]() {
        if (webSocket->state() == QAbstractSocket::ConnectedState)
            webSocket->ping();
    });
}

void Engine::WebSocket::cleanup() {
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

void Engine::WebSocket::connectToStream() {
    if (isConnecting || webSocket->state() == QAbstractSocket::ConnectedState)
        return;

    isConnecting = true;

    webSocket->open(Url);
}

void Engine::WebSocket::disconnectFromStream() {
    autoReconnect = false;
    isConnecting = false;

    if (reconnectTimer && reconnectTimer->isActive())
        reconnectTimer->stop();

    if (pingTimer && pingTimer->isActive())
        pingTimer->stop();

    if (webSocket && webSocket->state() != QAbstractSocket::UnconnectedState) {
        unsubscribeAllCoins();
        webSocket->close();
        webSocket->abort();
    }
}

void Engine::WebSocket::subscribeToCoins(const QStringList &coins) {
    QWriteLocker locker(&dataLock);

    if (coins.isEmpty())
        return;

    for (const QString& coin : coins) {
        subscribedCoins.insert(coin.toUpper());
        usedStreams.insert(tickerStream(coin));
        usedStreams.insert(orderBooksStream(coin));
    }

    locker.unlock();

    if (isConnected())
        sendSubscriptionMessage(usedStreams.values());
}

void Engine::WebSocket::unsubscribeFromCoins(const QStringList &coins) {
    if (coins.isEmpty())
        return;

    QStringList streamsToUnsubscribe;
    QWriteLocker locker(&dataLock);

    for (const QString& coin : coins) {
        QString coinUpper = coin.toUpper();
        if (subscribedCoins.contains(coinUpper)) {
            subscribedCoins.remove(coinUpper);

            QString stream = tickerStream(coin);
            usedStreams.remove(stream);
            streamsToUnsubscribe.append(stream);

            stream = orderBooksStream(coin);
            usedStreams.remove(stream);
            streamsToUnsubscribe.append(stream);
        }
    }
    locker.unlock();

    if (isConnected() && !streamsToUnsubscribe.isEmpty())
        sendUnsubscriptionMessage(streamsToUnsubscribe);
}

void Engine::WebSocket::unsubscribeAllCoins() {
    QWriteLocker locker(&dataLock);

    if (usedStreams.isEmpty())
        return;

    QStringList listUsedStreams = usedStreams.values();

    subscribedCoins.clear();
    usedStreams.clear();

    locker.unlock();


    if (isConnected())
        sendUnsubscriptionMessage(listUsedStreams);
}

QStringList Engine::WebSocket::getSubscribedCoins() {
    QReadLocker locker(&dataLock);
    return subscribedCoins.values();
}

bool Engine::WebSocket::isConnected() const {
    return webSocket && webSocket->state() == QAbstractSocket::ConnectedState;
}

void Engine::WebSocket::onConnected() {
    qDebug() << "ID : " << uniqueId << "Connected to WebSocket";
    isConnecting = false;
    reconnectAttempts = 0;

    pingTimer->start(30000);

    if (!usedStreams.isEmpty())
        QTimer::singleShot(100, this, [this]() {
            sendSubscriptionMessage(usedStreams.values());
        });

    emit connected();
}

void Engine::WebSocket::onTextMessageReceived(const QString &message) {
    qDebug() << "ID : " << uniqueId << " Received : " << message;
    auto jsonObj = parseTextMessage(message);
    if (jsonObj.has_value()) {
        messageReceived(jsonObj.value());
        sendPingMessage(jsonObj.value());
    }
    else
        qWarning() << jsonObj.error();
}

void Engine::WebSocket::onDisconnected() {
    qDebug() << "ID : " << uniqueId << " Disconnected from WebSocket";
    if (pingTimer && pingTimer->isActive())
        pingTimer->stop();

    emit disconnected();

    if (autoReconnect && reconnectAttempts < 10) {
        int delay = qMin(60000, (1 << reconnectAttempts) * 1000);
        reconnectTimer->start(delay);
        reconnectAttempts++;
    }
}

void Engine::WebSocket::onError(QAbstractSocket::SocketError error) {
    isConnecting = false;
    QString errorString = webSocket->errorString();
    qDebug() << "ID : " << uniqueId << " WebSocket error:" << error << "-" << errorString;
    emit errorOccurred(errorString);
}

void Engine::WebSocket::onSslErrors(const QList<QSslError> &errors) {
    qDebug() << "ID : " << uniqueId << " SSL errors occurred:";
    for (const QSslError &error : errors)
        qDebug() << " -" << error.errorString();

    webSocket->ignoreSslErrors();
}

void Engine::WebSocket::reconnect() {
    if (autoReconnect && !isConnecting)
        connectToStream();
}

std::expected<QJsonObject, QString> Engine::WebSocket::parseTextMessage(const QString &message) {
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8(), &parseError);

    if (parseError.error != QJsonParseError::NoError)
        return std::unexpected(QString("ID : %1 - JSON parse error: %2").arg(uniqueId).arg(parseError.errorString()));

    if (!doc.isObject()) 
        return std::unexpected(QString("ID : %1 - Document is not an object").arg(uniqueId));

    return doc.object();
}

void Engine::WebSocket::sendPingMessage(const QJsonObject& obj) {
    if (obj.contains("ping")) {
        QJsonObject pong{{"pong", obj["ping"]}};
        webSocket->sendTextMessage(QJsonDocument(pong).toJson());
    }
}