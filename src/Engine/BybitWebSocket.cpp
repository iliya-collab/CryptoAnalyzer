#include "Engine/BybitWebSocket.hpp"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

namespace Engine {

    BybitWebSocket::BybitWebSocket(QObject* parent) : QObject(parent), m_webSocket(nullptr), m_pingTimer(nullptr) {
        // Настраиваем websocket
        setupWebSocket();
        // Настраивием соединения с websocket
        setupConnections();
    }

    BybitWebSocket::~BybitWebSocket() {
        // Корректное закрытие websocket с отключением всех соединений и таймеров
        cleanup();
    }

    void BybitWebSocket::setupWebSocket() {
        QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
        sslConfig.setPeerVerifyMode(QSslSocket::VerifyPeer);
        sslConfig.setProtocol(QSsl::TlsV1_2OrLater);

        m_pingTimer = new QTimer(this);
        m_webSocket = new QWebSocket("", QWebSocketProtocol::VersionLatest, this);

        m_webSocket->setSslConfiguration(sslConfig);
    }

    void BybitWebSocket::setupConnections() {
        connect(m_webSocket, &QWebSocket::connected, this, &BybitWebSocket::onConnected);
        connect(m_webSocket, &QWebSocket::disconnected, this, &BybitWebSocket::onDisconnected);
        connect(m_webSocket, &QWebSocket::textMessageReceived, this, &BybitWebSocket::onTextMessageReceived);
        connect(m_webSocket, &QWebSocket::errorOccurred, this, &BybitWebSocket::onError);
        connect(m_webSocket, &QWebSocket::sslErrors, this, &BybitWebSocket::onSslErrors);


        connect(m_pingTimer, &QTimer::timeout, this, [this]() {
            if (isOpen())
                m_webSocket->ping();
        });
    }

    void BybitWebSocket::cleanup() {
        if (m_pingTimer) {
            m_pingTimer->blockSignals(true);
            if (m_pingTimer->isActive())
                m_pingTimer->stop();
            m_pingTimer->disconnect(this);
        }

        if (m_webSocket) {
            if (isOpen()) {
                disconnectFromStream();
                m_webSocket->close();
            }
            m_webSocket->disconnect(this);
        }
    }

    bool BybitWebSocket::isOpen() {
        return m_webSocket->state() == QAbstractSocket::ConnectedState;
    }

    void BybitWebSocket::open(const QUrl& baseEndpont) {
        m_webSocket->open(baseEndpont);
    }

    void BybitWebSocket::close() {
        if (m_pingTimer && m_pingTimer->isActive())
            m_pingTimer->stop();

        if (m_webSocket && isOpen())
            m_webSocket->close();
    }

    void BybitWebSocket::connectToStream() {    
        if (!m_usedStreams.isEmpty())
            sendSubscriptionMessage(m_usedStreams.values());
    }

    void BybitWebSocket::disconnectFromStream() {
        if (!m_usedStreams.isEmpty()) {
            sendUnsubscriptionMessage(m_usedStreams.values());
            m_usedStreams.clear();
        }
    }

    void BybitWebSocket::subscribeToStream(const QString& coin, QSet<Stream> streams) {
        for (auto stream : streams)
            switch (stream) {
            case Stream::Ticker:
                m_usedStreams.insert(createTickerStream(coin));
                break;
            case Stream::Orderbook:
                m_usedStreams.insert(createOrderbookStream(coin));
            default:
                break;
            }
    }

    void BybitWebSocket::onConnected() {
        // Запускаем таймер ping
        m_pingTimer->start(ACTIVE_PING_INTERVAL);
        emit connected();
    }

    void BybitWebSocket::onDisconnected() {
        // Отключаем таймер ping
        if (m_pingTimer && m_pingTimer->isActive())
            m_pingTimer->stop();
        emit disconnected();
    }

    void BybitWebSocket::onTextMessageReceived(const QString &message) {
        auto jsonObj = parseTextMessage(message);
        if (jsonObj.has_value()) {
            messageReceived(jsonObj.value());
            sendPingMessage(jsonObj.value());
        }
        else
            emit errorOccurred(jsonObj.error());
    }

    void BybitWebSocket::onError(QAbstractSocket::SocketError error) {
        emit errorOccurred(m_webSocket->errorString());
    }

    void BybitWebSocket::onSslErrors(const QList<QSslError>& errors) {
        QStringList errorStrings;
        bool fatal = false;

        for (const QSslError& error : errors) {
            errorStrings << error.errorString();
            fatal = error.error() == QSslError::CertificateExpired ||
                    error.error() == QSslError::CertificateNotYetValid ||
                    error.error() == QSslError::CertificateUntrusted ||
                    error.error() == QSslError::HostNameMismatch;
        }

        emit errorOccurred("SSL errors: " + errorStrings.join(", "));

        if (fatal)
            m_webSocket->abort();
    }

    std::expected<QJsonObject, QString> BybitWebSocket::parseTextMessage(const QString &message) {
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8(), &parseError);

        if (parseError.error != QJsonParseError::NoError)
            return std::unexpected(parseError.errorString());

        if (!doc.isObject()) 
            return std::unexpected("Document is not an object");

        return doc.object();
    }

    void BybitWebSocket::sendPingMessage(const QJsonObject& obj) {
        if (obj.contains("ping")) {
            QJsonObject pong{{"pong", obj["ping"]}};
            m_webSocket->sendTextMessage(QJsonDocument(pong).toJson());
        }
    }

    void BybitWebSocket::messageReceived(const QJsonObject &obj) {
        if (obj.contains("topic")) {
            QString channel = obj["topic"].toString();
            if (channel.startsWith("tickers."))
                updateTicker(obj);
            else if (channel.startsWith("orderbook."))
                updateOrderbook(obj);
        }
    }

    void BybitWebSocket::updateTicker(const QJsonObject &json) {
        if (!json.contains("data") || !json["data"].isObject())
            return;
            
        QJsonObject data = json["data"].toObject();
        QString symbol = data["symbol"].toString();

        stTicker ticker = {};

        ticker.symbol = symbol;

        if (data.contains("lastPrice"))
            ticker.lastPrice = data["lastPrice"].toString().toDouble();
        
        if (data.contains("highPrice24h") && data.contains("lowPrice24h") && data.contains("turnover24h") && data.contains("volume24h")) {
            ticker.high24h = data["highPrice24h"].toString().toDouble();
            ticker.low24h = data["lowPrice24h"].toString().toDouble();
            ticker.volCcy24h = data["turnover24h"].toString().toDouble();
            ticker.vol24h = data["volume24h"].toString().toDouble();
        }

        emit updatedTicker(ticker);
    }

    void BybitWebSocket::updateOrderbook(const QJsonObject &json) {
        if (!json.contains("data") || !json["data"].isObject())
            return;
            
        QJsonObject data = json["data"].toObject();
        QString symbol = data["symbol"].toString();
        QString type = json["type"].toString();
        
        quint64 updateId = data.value("u").toVariant().toULongLong();
        quint64 seq = data.value("seq").toVariant().toULongLong();
        
        static stOrderBooks orderBooks = {};

        QWriteLocker locker(&m_dataLock);
        
        if (type == "snapshot") {
            orderBooks.bids.clear();
            orderBooks.asks.clear();
        } else if (updateId <= orderBooks.lastUpdateId)
            return;
        
        orderBooks.lastUpdateId = updateId;
        orderBooks.lastSeq = seq;
    
        QJsonArray bidsArray = data.value("b").toArray();
        for (int i = 0; i < bidsArray.size(); i++) {
            QJsonArray bid = bidsArray[i].toArray();

            double price = bid[0].toString().toDouble();
            double size = bid[1].toString().toDouble();

            if (qFuzzyIsNull(size) || size <= 0)
                orderBooks.bids.remove(price);
            else
                orderBooks.bids[price] = size;

        }

        QJsonArray asksArray = data.value("a").toArray();
        for (int i = 0; i < asksArray.size(); i++) {
            QJsonArray ask = asksArray[i].toArray();

            double price = ask[0].toString().toDouble();
            double size = ask[1].toString().toDouble();

            if (qFuzzyIsNull(size) || size <= 0)
                orderBooks.asks.remove(price);
            else
                orderBooks.asks[price] = size;
        }

        emit updatedOrderbook(orderBooks);
    }

    void BybitWebSocket::sendSubscriptionMessage(const QStringList &streams) {
        for (int i = 0; i < streams.size(); i += MAX_STREAMS_PER_SUBSCRIPTION) {
            QStringList chunk = streams.mid(i, MAX_STREAMS_PER_SUBSCRIPTION);

            QJsonObject subscribeMessage;
            subscribeMessage["op"] = "subscribe";
            subscribeMessage["args"] = QJsonArray::fromStringList(chunk);
            subscribeMessage["req_id"] = QString::number(i + 1);

            QJsonDocument doc(subscribeMessage);
            QString message = doc.toJson(QJsonDocument::Compact);

            m_webSocket->sendTextMessage(message);

            if (i + MAX_STREAMS_PER_SUBSCRIPTION < streams.size())
                QThread::msleep(100);
        }
    }

    void BybitWebSocket::sendUnsubscriptionMessage(const QStringList &streams) {
        for (int i = 0; i < streams.size(); i += MAX_STREAMS_PER_SUBSCRIPTION) {
            QStringList chunk = streams.mid(i, MAX_STREAMS_PER_SUBSCRIPTION);

            QJsonObject unsubscribeMessage;
            unsubscribeMessage["op"] = "unsubscribe";
            unsubscribeMessage["args"] = QJsonArray::fromStringList(chunk);
            unsubscribeMessage["req_id"] = QString::number(1000 + i);

            QJsonDocument doc(unsubscribeMessage);
            QString message = doc.toJson(QJsonDocument::Compact);

            m_webSocket->sendTextMessage(message);

            if (i + MAX_STREAMS_PER_SUBSCRIPTION < streams.size())
                QThread::msleep(100);
        }

    }

    QString BybitWebSocket::createTickerStream(const QString &coin) {
        return QString("tickers.%1").arg(coin);
    }

    QString BybitWebSocket::createOrderbookStream(const QString &coin) {
        return QString("orderbook.50.%1").arg(coin);
    }

}