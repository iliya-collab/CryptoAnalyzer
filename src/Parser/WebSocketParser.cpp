#include "Parser/WebSocketParser.hpp"

WebSocketParser::WebSocketParser(const QString& name, QObject* parent) :
    QObject(parent),
    nameMarket(name),
    webSocket(nullptr),
    reconnectTimer(nullptr),
    pingTimer(nullptr),
    autoReconnect(true),
    reconnectAttempts(0),
    isConnecting(false),
    isCorrectInit(true),
    t_market(TMarketData::NONEMARKET) {}

bool WebSocketParser::init() {
    setupWebSocket();
    connectSignals();

    t_market = getTypeMarket(nameMarket);
    WebSocketUrl();

    pingTimer->start(ACTIVE_PING_INTERVAL);

    return isCorrectInit;
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

WebSocketParser::TMarketData WebSocketParser::getTypeMarket(const QString& name) {
    QStringList tocken = name.split("/");
    if (tocken.size() != 2)
        return TMarketData::NONEMARKET;
    else if (tocken[1] == "spot")
        return TMarketData::SPOT;
    else if (tocken[1] == "futures")
        return TMarketData::FUTURES;
    else
        return TMarketData::NONEMARKET;
}

void WebSocketParser::WebSocketUrl() {
    auto ret = getURLMarketData();
    if (ret.has_value())
        Url = ret.value();
    else {
        isCorrectInit = false;
        qCritical() << "Failed to initialize WebSocketParser:" << ret.error();
    }
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
        unsubscribeAllCoins();
        webSocket->close();
        webSocket->abort();
    }
}

void WebSocketParser::subscribeToCoins(const QStringList &coins) {
    QWriteLocker locker(&dataLock);

    if (coins.isEmpty())
        return;

    for (const QString& coin : coins) {
        subscribedCoins.insert(coin.toUpper());
        if (_Channels & Channel::TICKER)
            usedStreams.insert(tickerStream(coin));
        if (_Channels & Channel::BOOKS5)
            usedStreams.insert(books5Stream(coin));
        if (_Channels & Channel::BOOKS10)
            usedStreams.insert(books10Stream(coin));
        if (_Channels & Channel::BOOKS20)
            usedStreams.insert(books20Stream(coin));
    }

    locker.unlock();

    if (isConnected())
        sendSubscriptionMessage(usedStreams.values());
}

void WebSocketParser::unsubscribeFromCoins(const QStringList &coins) {
    if (coins.isEmpty())
        return;

    QStringList streamsToUnsubscribe;
    QWriteLocker locker(&dataLock);

    for (const QString& coin : coins) {
        QString coinUpper = coin.toUpper();
        if (subscribedCoins.contains(coinUpper)) {
            subscribedCoins.remove(coinUpper);
            currentInfoAboutCoins.remove(coinUpper);

            QString stream = tickerStream(coin);

            usedStreams.remove(stream);
            streamsToUnsubscribe.append(stream);
        }
    }
    locker.unlock();

    if (isConnected() && !streamsToUnsubscribe.isEmpty())
        sendUnsubscriptionMessage(streamsToUnsubscribe);
}

void WebSocketParser::unsubscribeAllCoins() {
    QWriteLocker locker(&dataLock);

    if (usedStreams.isEmpty())
        return;

    QStringList listUsedStreams = usedStreams.values();

    subscribedCoins.clear();
    currentInfoAboutCoins.clear();
    usedStreams.clear();

    locker.unlock();


    if (isConnected())
        sendUnsubscriptionMessage(listUsedStreams);
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

    if (!usedStreams.isEmpty())
        QTimer::singleShot(100, this, [this]() {
            sendSubscriptionMessage(usedStreams.values());
        });

    emit connected();
}

void WebSocketParser::onTextMessageReceived(const QString &message) {
    qDebug() << QString("%1:%2").arg(getNameMarket()).arg(message);
    auto jsonObj = parseTextMessage(message);
    if (jsonObj.has_value()) {
        messageReceived(jsonObj.value());
        sendPingMessage(jsonObj.value());
    }
    else
        qWarning() << jsonObj.error();
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

QString WebSocketParser::getNameMarket() {
    return nameMarket;
}

int WebSocketParser::getChannel(const QString& channel) {
    if (channel == "ticker")
        return Channel::TICKER;
    else if (channel == "books5")
        return Channel::BOOKS5;
    else if (channel == "books10")
        return Channel::BOOKS10;
    else if (channel == "books20")
        return Channel::BOOKS20;
    return Channel::NONECHANNEL;
}

void WebSocketParser::addChannels(const QString& channels) {
    QStringList lst_channels = channels.split("/");
    for (auto& channel : lst_channels)
        _Channels |= getChannel(channel);
}

void WebSocketParser::deleteChannels(const QString& channels) {
    QStringList lst_channels = channels.split("/");
    for (auto& channel : lst_channels)
        _Channels ^= getChannel(channel);
}

void WebSocketParser::deleteAllChannels() {
    _Channels = 0;
}

std::expected<QJsonObject, QString> WebSocketParser::parseTextMessage(const QString &message) {
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8(), &parseError);

    if (parseError.error != QJsonParseError::NoError)
        return std::unexpected(QString("%1 : JSON parse error: %2").arg(getNameMarket()).arg(parseError.errorString()));

    if (!doc.isObject()) 
        return std::unexpected(QString("%1 : Document is not an object").arg(getNameMarket()));

    return doc.object();
}

void WebSocketParser::sendPingMessage(const QJsonObject& obj) {
    if (obj.contains("ping")) {
        QJsonObject pong{{"pong", obj["ping"]}};
        webSocket->sendTextMessage(QJsonDocument(pong).toJson());
    }
}