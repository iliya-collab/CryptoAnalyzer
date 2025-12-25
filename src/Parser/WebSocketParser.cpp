#include "Parser/WebSocketParser.hpp"

QMap<QString, WebSocketParser::stInfoCoin> WebSocketParser::currentInfoAboutCoins;
QReadWriteLock WebSocketParser::dataLock;
QSet<QString> WebSocketParser::subscribedCoins;
QUrl WebSocketParser::TradeUrl;

WebSocketParser::WebSocketParser(QObject* parent) :
    QObject(parent), 
    webSocket(nullptr), 
    reconnectTimer(nullptr), 
    pingTimer(nullptr),
    autoReconnect(true), 
    reconnectAttempts(0), 
    isConnecting(false)
{
    setupWebSocket();
}
WebSocketParser::~WebSocketParser() {
    cleanup();
}

void WebSocketParser::setupWebSocket() {
    // НАСТРОЙКА SSL
    //QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
    //sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
    QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
    sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
    sslConfig.setProtocol(QSsl::TlsV1_2OrLater);

    reconnectTimer = std::make_unique<QTimer>();
    pingTimer = std::make_unique<QTimer>();
    webSocket = std::make_unique<QWebSocket>("", QWebSocketProtocol::VersionLatest);

    webSocket->setSslConfiguration(sslConfig);
    

    // ПОДКЛЮЧЕНИЕ СИГНАЛОВ
    connect(webSocket.get(), &QWebSocket::connected, this, &WebSocketParser::onConnected);
    connect(webSocket.get(), &QWebSocket::disconnected, this, &WebSocketParser::onDisconnected);
    connect(webSocket.get(), &QWebSocket::textMessageReceived, this, &WebSocketParser::onTextMessageReceived);
    connect(webSocket.get(), &QWebSocket::errorOccurred, this, &WebSocketParser::onError);
    connect(webSocket.get(), &QWebSocket::sslErrors, this, &WebSocketParser::onSslErrors);

    // ТАЙМЕР ПЕРЕПОДКЛЮЧЕНИЯ
    reconnectTimer->setSingleShot(true);
    connect(reconnectTimer.get(), &QTimer::timeout, this, &WebSocketParser::reconnect);

    // ТАЙМЕР PING (поддержание соединения)
    connect(pingTimer.get(), &QTimer::timeout, this, [this]() {
        if (webSocket->state() == QAbstractSocket::ConnectedState)
            webSocket->ping();
    });
}

void WebSocketParser::cleanup() {
    // Отключаем автореконнект
    autoReconnect = false;
    isConnecting = false;
    
    // Безопасно останавливаем таймеры
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
    
    // Безопасно закрываем WebSocket
    if (webSocket) {
        webSocket->disconnect(this);
        if (webSocket->state() != QAbstractSocket::UnconnectedState)
            webSocket->close();
    }
}

void WebSocketParser::connectToStream(QUrl url) {
    if (isConnecting || webSocket->state() == QAbstractSocket::ConnectedState)
        return;

    isConnecting = true;

    // Дополнительные настройки URL при необходимости
    webSocket->open(url);
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
        // В ручном режиме можно использовать abort()
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
    
    // ЕСЛИ УЖЕ ПОДКЛЮЧЕНЫ - ОТПРАВЛЯЕМ ПОДПИСКУ СРАЗУ
    if (isConnected()) 
        sendSubscriptionMessage();
}

void WebSocketParser::unsubscribeFromCoins(const QStringList &coins) {
    if (coins.isEmpty()) 
        return;

    // ПОДГОТАВЛИВАЕМ СПИСОК STREAMS ДЛЯ ОТПИСКИ
    QStringList streamsToUnsubscribe;
    QWriteLocker locker(&dataLock);
    
    for (const QString &coin : coins) {
        QString coinUpper = coin.toUpper();
        if (subscribedCoins.contains(coinUpper)) {
            // Удаляем из множества подписанных монет
            subscribedCoins.remove(coinUpper);
            
            // Удаляем текущую цену
            currentInfoAboutCoins.remove(coinUpper);
            
            // Добавляем stream для отписки
            streamsToUnsubscribe.append(coinToStream(coin));
        }
    }
    locker.unlock();

    // ОТПРАВЛЯЕМ ЗАПРОС НА ОТПИСКУ ЕСЛИ ПОДКЛЮЧЕНЫ
    if (isConnected() && !streamsToUnsubscribe.isEmpty())
        sendUnsubscriptionMessage(streamsToUnsubscribe);
}

void WebSocketParser::unsubscribeAllCoins() {
     QWriteLocker locker(&dataLock);
    
    if (subscribedCoins.isEmpty())
        return;
    
    // ПОДГОТАВЛИВАЕМ ВСЕ STREAMS ДЛЯ ОТПИСКИ
    QStringList allStreams;
    for (const QString &coin : subscribedCoins)
        allStreams.append(coinToStream(coin));
    
    // ОЧИЩАЕМ ДАННЫЕ
    subscribedCoins.clear();
    currentInfoAboutCoins.clear();
    
    locker.unlock();

    // ОТПРАВЛЯЕМ ОТПИСКУ ОТ ВСЕХ STREAMS
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
    isConnecting = false;
    reconnectAttempts = 0;
    
    // ЗАПУСКАЕМ PING ДЛЯ ПОДДЕРЖАНИЯ СОЕДИНЕНИЯ
    pingTimer->start(30000); // Ping каждые 30 секунд
    
    // ПОДПИСКА НА ДАННЫЕ ПОСЛЕ ПОДКЛЮЧЕНИЯ
    if (!subscribedCoins.isEmpty())
        QTimer::singleShot(100, this, &WebSocketParser::sendSubscriptionMessage);
    
    emit connected();
}

void WebSocketParser::onDisconnected() {
    if (pingTimer && pingTimer->isActive())
        pingTimer->stop();

    emit disconnected();
    
    // ПЕРЕПОДКЛЮЧЕНИЕ С ЭКСПОНЕНЦИАЛЬНОЙ ЗАДЕРЖКОЙ
    if (autoReconnect && reconnectAttempts < 10) {
        int delay = qMin(60000, (1 << reconnectAttempts) * 1000); // Макс 1 минута
        reconnectTimer->start(delay);
        reconnectAttempts++;
    }
}

void WebSocketParser::onError(QAbstractSocket::SocketError error) {
    isConnecting = false;
    QString errorString = webSocket->errorString();
    qDebug() << "WebSocket error:" << error << "-" << errorString;
    emit errorOccurred(errorString);
}

void WebSocketParser::onSslErrors(const QList<QSslError> &errors) {
    qDebug() << "SSL errors occurred:";
    for (const QSslError &error : errors) {
        qDebug() << " -" << error.errorString();
    }
    
    // 🔧 ИГНОРИРУЕМ SSL ОШИБКИ ДЛЯ ТЕСТИРОВАНИЯ
    webSocket->ignoreSslErrors();
}

void WebSocketParser::reconnect() {
    if (autoReconnect && !isConnecting)
        connectToStream(TradeUrl);
}