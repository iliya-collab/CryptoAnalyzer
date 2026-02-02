#include "Parser/BybitParser.hpp"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <optional>

//https://chat.deepseek.com/share/qpzt4nu5edyvbmz7ok

std::expected<QUrl, QString> BybitParser::getURLMarketData() {
    switch (t_market) {
        case TMarketData::SPOT:
            return QUrl("wss://stream.bybit.com/v5/public/spot");
        case TMarketData::FUTURES:
            return QUrl("wss://stream.bybit.com/v5/public/linear");
        default:
            return std::unexpected(QString("The name is incorrect : %1").arg(nameMarket));
    }
}

QString BybitParser::formatCoin(const QString& coin) {
    return coin;
}

void BybitParser::messageReceived(const QJsonObject &obj) {
    if (obj.contains("topic")) {
        QString channel = obj["topic"].toString();
        if (channel.startsWith("tickers."))
            updateTicker(obj);
        else if (channel.startsWith("orderbook."))
            updateOrderBooks(obj);
    }
}

void BybitParser::updateTicker(const QJsonObject &json) {
    //qDebug() << QJsonDocument(json).toJson(QJsonDocument::Compact);
    if (!json.contains("data") || !json["data"].isObject())
        return;
        
    QJsonObject data = json["data"].toObject();
    QString coin = formatCoin(data["symbol"].toString());

    stTicker ticker = {};

    ticker.namePair = QString("%1:%2").arg(getNameMarket()).arg(coin);

    if (data.contains("lastPrice"))
        ticker.curPrice = data["lastPrice"].toString().toDouble();

    if (data.contains("ask1Price") && data.contains("ask1Size") && data.contains("bid1Price") && data.contains("bid1Size")) {
        ticker.bestAsk = {
            .askPrice = data["ask1Price"].toString().toDouble(),
            .askSize = data["ask1Size"].toString().toDouble()
        };
        ticker.bestBid = {
            .bidPrice = data["bid1Price"].toString().toDouble(),
            .bidSize  = data["bid1Size"].toString().toDouble(),
        };
        ticker.spread   = ticker.bestAsk.askPrice - ticker.bestBid.bidPrice;
    }
    
    if (data.contains("highPrice24h") && data.contains("lowPrice24h") && data.contains("turnover24h") && data.contains("volume24h")) {
        ticker.high24h = data["highPrice24h"].toString().toDouble();
        ticker.low24h = data["lowPrice24h"].toString().toDouble();
        ticker.volCcy24h = data["turnover24h"].toString().toDouble();
        ticker.vol24h = data["volume24h"].toString().toDouble();
    }

    emit updatedTicker(ticker);
}

void BybitParser::updateOrderBooks(const QJsonObject &json) {
    if (!json.contains("data") || !json["data"].isObject())
        return;
        
    QJsonObject data = json["data"].toObject();
    QString coin = formatCoin(data["symbol"].toString());

    stOrderBooks order_books = {};

    order_books.namePair = QString("%1:%2").arg(getNameMarket()).arg(coin);
    
    QJsonArray bidsArray = data.value("b").toArray();
    for (int i = 0; i < bidsArray.size() && i < MAX_DEPTH_BOOKS; i++) {
        QJsonArray bid = bidsArray[i].toArray();
        if (bid.size() >= 2) {
            Bid _bid = {
                .bidPrice = bid[0].toString().toDouble(),
                .bidSize  = bid[1].toString().toDouble(),
            };
            order_books.totalBidVolume += _bid.bidSize;
            order_books.bids.append(_bid);
        }
    }

    QJsonArray asksArray = data.value("a").toArray();
    for (int i = 0; i < asksArray.size() && i < MAX_DEPTH_BOOKS; i++) {
        QJsonArray ask = asksArray[i].toArray();
        if (ask.size() >= 2) {
            Ask _ask = {
                .askPrice = ask[0].toString().toDouble(),
                .askSize  = ask[1].toString().toDouble()
            };
            order_books.totalAskVolume += _ask.askSize;
            order_books.asks.append(_ask);
        }
    }

    if (!order_books.asks.isEmpty() && !order_books.bids.isEmpty()) 
        order_books.spread = order_books.asks.first().askPrice - order_books.bids.first().bidPrice;
    else
        order_books.spread = 0.0;

    emit updatedOrderBooks(order_books);
}

void BybitParser::sendSubscriptionMessage(const QStringList &streams) {
    for (int i = 0; i < streams.size(); i += MAX_STREAMS_PER_SUBSCRIPTION) {
        QStringList chunk = streams.mid(i, MAX_STREAMS_PER_SUBSCRIPTION);

        QJsonObject subscribeMessage;
        subscribeMessage["op"] = "subscribe";
        subscribeMessage["args"] = QJsonArray::fromStringList(chunk);
        subscribeMessage["req_id"] = QString::number(i + 1);

        QJsonDocument doc(subscribeMessage);
        QString message = doc.toJson(QJsonDocument::Compact);

        webSocket->sendTextMessage(message);

        if (i + MAX_STREAMS_PER_SUBSCRIPTION < streams.size())
            QThread::msleep(100);
    }
}

void BybitParser::sendUnsubscriptionMessage(const QStringList &streams) {
    for (int i = 0; i < streams.size(); i += MAX_STREAMS_PER_SUBSCRIPTION) {
        QStringList chunk = streams.mid(i, MAX_STREAMS_PER_SUBSCRIPTION);

        QJsonObject unsubscribeMessage;
        unsubscribeMessage["op"] = "unsubscribe";
        unsubscribeMessage["args"] = QJsonArray::fromStringList(chunk);
        unsubscribeMessage["req_id"] = QString::number(1000 + i);

        QJsonDocument doc(unsubscribeMessage);
        QString message = doc.toJson(QJsonDocument::Compact);

        webSocket->sendTextMessage(message);

        if (i + MAX_STREAMS_PER_SUBSCRIPTION < streams.size())
            QThread::msleep(100);
    }

}

QString BybitParser::tickerStream(const QString &coin) {
    QString _coin = coin;
    return QString("tickers.%1").arg(_coin.replace("/", "").toUpper());
}

QString BybitParser::orderBooksStream(const QString &coin) {
    QString _coin = coin;
    return QString("orderbook.50.%1").arg(_coin.replace("/", "").toUpper());
}