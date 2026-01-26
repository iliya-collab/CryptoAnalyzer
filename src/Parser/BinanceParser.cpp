#include "Parser/BinanceParser.hpp"

std::expected<QUrl, QString> BinanceParser::getURLMarketData() {
    switch (t_market) {
        case TMarketData::SPOT:
            return QUrl("wss://stream.binance.com/ws");
        case TMarketData::FUTURES:
            return QUrl("wss://fstream.binance.com/ws");
        default:
            return std::unexpected(QString("The name is incorrect : %1").arg(nameMarket));
    }
}

QString BinanceParser::formatCoin(const QString& coin) {
    return coin;
}

void BinanceParser::messageReceived(const QJsonObject &obj)  {
    if (obj.contains("e")) {
        QString channel = obj["e"].toString();
        if (channel == "24hrTicker")
            updateTicker(obj);
        else if (channel == "depthUpdate")
            updateOrderBooks(obj);
    }

}

void BinanceParser::updateTicker(const QJsonObject &json) {
    if (!json.contains("s"))
        return;

    QString coin = json["s"].toString();

    stTicker ticker = {};

    ticker.namePair = QString("%1:%2").arg(getNameMarket()).arg(coin);

    if (json.contains("c")) 
        ticker.curPrice = json["c"].toString().toDouble();

    if (json.contains("a") && json.contains("A") && json.contains("b") && json.contains("B")) {
        ticker.bestAsk = {
            .askPrice = json["a"].toString().toDouble(),
            .askSize = json["A"].toString().toDouble()
        };
        ticker.bestBid = {
            .bidPrice = json["b"].toString().toDouble(),
            .bidSize  = json["B"].toString().toDouble(),
        };
        ticker.spread   = ticker.bestAsk.askPrice - ticker.bestBid.bidPrice;
    }
    
    if (json.contains("h") && json.contains("l") && json.contains("q") && json.contains("v")) {
        ticker.high24h = json["h"].toString().toDouble();
        ticker.low24h = json["l"].toString().toDouble();
        ticker.volCcy24h = json["q"].toString().toDouble();
        ticker.vol24h = json["v"].toString().toDouble();
    }

    updatedTicker(ticker);

}

void BinanceParser::updateOrderBooks(const QJsonObject &json) {
    if (!json.contains("s"))
        return;

    QString coin = formatCoin(json["s"].toString());

    stOrderBooks order_books = {};

    order_books.namePair = QString("%1:%2").arg(getNameMarket()).arg(coin);

    QJsonArray bidsArray = json.value("b").toArray();
    for (int i = 0; i < bidsArray.size() && i < mxDepthBooks; i++) {
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

    QJsonArray asksArray = json.value("a").toArray();
    for (int i = 0; i < asksArray.size() && i < mxDepthBooks; i++) {
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

void BinanceParser::sendSubscriptionMessage(const QStringList &streams) {
    for (int i = 0; i < streams.size(); i += MAX_STREAMS_PER_SUBSCRIPTION) {
        QStringList chunk = streams.mid(i, MAX_STREAMS_PER_SUBSCRIPTION);

        QJsonObject subscribeMessage;
        subscribeMessage["method"] = "SUBSCRIBE";
        subscribeMessage["params"] = QJsonArray::fromStringList(chunk);
        subscribeMessage["id"] = i + 1;

        QJsonDocument doc(subscribeMessage);
        QString message = doc.toJson(QJsonDocument::Compact);

        webSocket->sendTextMessage(message);

        if (i + MAX_STREAMS_PER_SUBSCRIPTION < streams.size())
            QThread::msleep(100);
    }
}

void BinanceParser::sendUnsubscriptionMessage(const QStringList &streams) {
    for (int i = 0; i < streams.size(); i += MAX_STREAMS_PER_SUBSCRIPTION) {
        QStringList chunk = streams.mid(i, MAX_STREAMS_PER_SUBSCRIPTION);

        QJsonObject unsubscribeMessage;
        unsubscribeMessage["method"] = "UNSUBSCRIBE";
        unsubscribeMessage["params"] = QJsonArray::fromStringList(chunk);
        unsubscribeMessage["id"] = 1000 + i;

        QJsonDocument doc(unsubscribeMessage);
        QString message = doc.toJson(QJsonDocument::Compact);

        webSocket->sendTextMessage(message);

        if (i + MAX_STREAMS_PER_SUBSCRIPTION < streams.size())
            QThread::msleep(100);
    }
}

QString BinanceParser::tickerStream(const QString &coin) {
    QString _coin = coin;
    return QString("%1@ticker").arg(_coin.replace("/", "").toLower());
}

QString BinanceParser::orderBooksStream(const QString &coin) {
    QString _coin = coin;
    return QString("%1@depth").arg(_coin.replace("/", "").toLower());
}