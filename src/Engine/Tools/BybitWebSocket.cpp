#include "Engine/Tools/BybitWebSocket.hpp"

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
        cleanup();
    }

    void BybitWebSocket::initAPI(const API& api) {
        m_api = api;
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
        connect(m_webSocket, &QWebSocket::bytesWritten, this, &BybitWebSocket::onBytesWritten);
        connect(m_webSocket, &QWebSocket::errorOccurred, this, &BybitWebSocket::onError);
        connect(m_webSocket, &QWebSocket::sslErrors, this, &BybitWebSocket::onSslErrors);

        connect(m_pingTimer, &QTimer::timeout, this, &BybitWebSocket::onPing);
    }

    void BybitWebSocket::cleanup() {
        if (m_pingTimer) {
            m_pingTimer->blockSignals(true);
            if (m_pingTimer->isActive())
                m_pingTimer->stop();
        }

        if (m_webSocket) {
            if (isOpen()) {
                disconnectFromStreams();
                closeAfterFlush();
            }
        }
    }

    bool BybitWebSocket::isOpen() {
        return m_webSocket && (m_webSocket->state() == QAbstractSocket::ConnectedState);
    }

    void BybitWebSocket::open() {
        QString url = m_api.m_isTestnet ? "wss://stream-testnet.bybit.com/v5/public/spot" : "wss://stream.bybit.com/v5/public/spot";
        m_webSocket->open(url);
    }

    void BybitWebSocket::close() {
        cleanup();
    }

    void BybitWebSocket::connectToStreams() {
        if (!m_usedStreams.isEmpty())
            sendSubscriptionMessage(m_usedStreams.values());
    }

    void BybitWebSocket::disconnectFromStreams() {
        if (!m_usedStreams.isEmpty()) {
            sendUnsubscriptionMessage(m_usedStreams.values());
            m_usedStreams.clear();
        }
    }

    void BybitWebSocket::closeAfterFlush() {
        m_webSocket->flush();
        if (m_webSocket->bytesToWrite() == 0)
            m_webSocket->close();
        else {
            m_pendingClose = true;
            QTimer::singleShot(5000, this, [this]() {
                if (m_pendingClose) {
                    m_pendingClose = false;
                    m_webSocket->close();
                }
            });
        }
    }

    void BybitWebSocket::subscribeToStream(const QString& coin, QSet<Stream> streams) {
        QStringList newStreams;
        QString streamName;

        for (auto stream : streams) {
            switch (stream) {
            case Stream::Ticker:
                streamName = createTickerStream(coin);
                break;
            case Stream::Orderbook:
                streamName = createOrderbookStream(coin);
                break;
            case Stream::Kline:
                break;
            }

            if (!m_usedStreams.contains(streamName)) {
                m_usedStreams.insert(streamName);
                newStreams << streamName;
            }
        }

        if (isOpen() && !newStreams.isEmpty())
            sendSubscriptionMessage(newStreams);
    }

    void BybitWebSocket::onConnected() {
        // Запускаем таймер ping
        m_pingTimer->start(ACTIVE_PING_INTERVAL);
        connectToStreams();
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
        if (jsonObj.has_value())
            messageReceived(jsonObj.value());
        else
            emit errorOccurred(jsonObj.error());
    }

    void BybitWebSocket::onBytesWritten(qint64 bytes) {
        if (m_pendingClose && (m_webSocket->bytesToWrite() == 0)) {
            m_pendingClose = false;
            m_webSocket->close();
        }
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

    void BybitWebSocket::onPing() {
        if (isOpen())
            sendPingMessage();
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

    void BybitWebSocket::messageReceived(const QJsonObject &obj) {
        //qDebug() << QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact));
        if (obj.contains("success")) {
            bool isSuccess = obj["success"].toBool();
            if (!isSuccess)
                emit errorOccurred(obj["op"].toString() + "failed: " + obj["ret_msg"].toString());
        } else if (obj.contains("topic")) {
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

        ticker.m_symbol = symbol;

        if (data.contains("lastPrice"))
            ticker.m_lastPrice = data["lastPrice"].toString().toDouble();

        if (data.contains("usdIndexPrice"))
            ticker.m_usdIndexPrice = data["usdIndexPrice"].toString().toDouble();
        
        if (data.contains("highPrice24h"))
            ticker.m_high24h = data["highPrice24h"].toString().toDouble();

        if (data.contains("lowPrice24h"))
            ticker.m_low24h = data["lowPrice24h"].toString().toDouble();

        if (data.contains("turnover24h"))
            ticker.m_volCcy24h = data["turnover24h"].toString().toDouble();

        if (data.contains("volume24h"))
            ticker.m_vol24h = data["volume24h"].toString().toDouble();

        if (data.contains("prevPrice24h"))
            ticker.m_prevPrice24h = ticker.m_lastPrice - data["prevPrice24h"].toString().toDouble();

        if (data.contains("price24hPcnt"))
            ticker.m_price24hPcnt = data["price24hPcnt"].toString().toDouble() * 100;

        emit updatedTicker(ticker);
    }

    void BybitWebSocket::updateOrderbook(const QJsonObject &json) {
        if (!json.contains("data") || !json["data"].isObject())
            return;
            
        QJsonObject data = json["data"].toObject();
        QString symbol = data["symbol"].toString();
        QString type = json["type"].toString();
        
        static stOrderBook orderBook = {};

        QWriteLocker locker(&m_dataLock);
        
        if (type == "snapshot") {
            orderBook.m_bids.clear();
            orderBook.m_asks.clear();
        }

        QJsonArray bidsArray = data.value("b").toArray();
        for (int i = 0; i < bidsArray.size(); i++) {
            QJsonArray bid = bidsArray[i].toArray();

            double price = bid[0].toString().toDouble();
            double size = bid[1].toString().toDouble();

            if (qFuzzyIsNull(size) || size <= 0)
                orderBook.m_bids.remove(price);
            else
                orderBook.m_bids[price] = size;

        }

        QJsonArray asksArray = data.value("a").toArray();
        for (int i = 0; i < asksArray.size(); i++) {
            QJsonArray ask = asksArray[i].toArray();

            double price = ask[0].toString().toDouble();
            double size = ask[1].toString().toDouble();

            if (qFuzzyIsNull(size) || size <= 0)
                orderBook.m_asks.remove(price);
            else
                orderBook.m_asks[price] = size;
        }

        emit updatedOrderbook(orderBook);
    }

    void BybitWebSocket::sendSubscriptionMessage(const QStringList &streams) {
        for (int i = 0; i < streams.size(); i += MAX_STREAMS_PER_SUBSCRIPTION) {
            QStringList chunk = streams.mid(i, MAX_STREAMS_PER_SUBSCRIPTION);

            QJsonObject subscribeMessage;
            subscribeMessage["op"] = "subscribe";
            subscribeMessage["args"] = QJsonArray::fromStringList(chunk);
            subscribeMessage["req_id"] = QString::number(m_nextReqId++);

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
            unsubscribeMessage["req_id"] = QString::number(m_nextReqId++);

            QJsonDocument doc(unsubscribeMessage);
            QString message = doc.toJson(QJsonDocument::Compact);

            m_webSocket->sendTextMessage(message);

            if (i + MAX_STREAMS_PER_SUBSCRIPTION < streams.size())
                QThread::msleep(100);
        }

    }

    void BybitWebSocket::sendPingMessage() {
        QJsonObject pingMessage;
        pingMessage["op"] = "ping";
        pingMessage["req_id"] = QString::number(m_nextReqId++);
        m_webSocket->sendTextMessage(QJsonDocument(pingMessage).toJson());

    }

    QString BybitWebSocket::createTickerStream(const QString &coin) {
        return QString("tickers.%1").arg(coin);
    }

    QString BybitWebSocket::createOrderbookStream(const QString &coin) {
        return QString("orderbook.50.%1").arg(coin);
    }

}