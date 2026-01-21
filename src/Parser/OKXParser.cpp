#include "Parser/OKXParser.hpp"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <optional>

std::expected<QUrl, QString> OKXParser::getURLMarketData() {
    switch (t_market) {
        case TMarketData::SPOT:
        case TMarketData::FUTURES:
            return QUrl("wss://ws.okx.com/ws/v5/public");
        default:
            return std::unexpected(QString("The name is incorrect : %1").arg(nameMarket));
    }
}

QString OKXParser::formatCoin(const QString& coin) {
    QStringList lst = coin.split('-');
    return lst[0]+lst[1];
}

void OKXParser::messageReceived(const QJsonObject &obj) {
    if (obj.contains("arg") && obj.contains("data")) {
        QJsonObject arg = obj["arg"].toObject();
        QString channel = arg["channel"].toString();
        if (channel == "tickers")
            updateTicker(obj);
        else if (channel == "books")
            updateOrderBooks(obj);
    }
}

void OKXParser::updateTicker(const QJsonObject &json) {
    QJsonObject arg = json["arg"].toObject();
    QString coin = formatCoin(arg["instId"].toString());

    QJsonArray dataArray = json["data"].toArray();
    if (dataArray.isEmpty())
        return;

    QJsonObject data = dataArray[0].toObject();

    stTicker ticker = {};

    ticker.namePair = QString("%1:%2").arg(getNameMarket()).arg(coin);

    if (data.contains("last"))
        ticker.curPrice = data["last"].toString().toDouble();

    if (data.contains("askPx") && data.contains("askSz") && data.contains("bidPx") && data.contains("bidSz")) {
        ticker.bestAsk = {
            .askPrice = json["askPx"].toString().toDouble(),
            .askSize = json["askSz"].toString().toDouble()
        };
        ticker.bestBid = {
            .bidPrice = json["bidPx"].toString().toDouble(),
            .bidSize  = json["bidSz"].toString().toDouble(),
        };
        ticker.spread   = ticker.bestAsk.askPrice - ticker.bestBid.bidPrice;
    }
    
    if (data.contains("high24h") && data.contains("low24h") && data.contains("volCcy24h") && data.contains("vol24h")) {
        ticker.high24h = data["high24h"].toString().toDouble();
        ticker.low24h = data["low24h"].toString().toDouble();
        ticker.volCcy24h = data["volCcy24h"].toString().toDouble();
        ticker.vol24h = data["vol24h"].toString().toDouble();
    }

    emit updatedTicker(ticker);
}

void OKXParser::updateOrderBooks(const QJsonObject &json) {
    QJsonObject arg = json["arg"].toObject();
    QString coin = formatCoin(arg["instId"].toString());

    QJsonArray dataArray = json["data"].toArray();
    if (dataArray.isEmpty())
        return;

    QJsonObject data = dataArray[0].toObject();

    stOrderBooks order_books = {};

    order_books.namePair = QString("%1:%2").arg(getNameMarket()).arg(coin);

    QJsonArray asksArray = data["asks"].toArray();
    QJsonArray bidsArray = data["bids"].toArray();
    for (int i = 0; i < bidsArray.size() && i < mxDepthBooks; i++) {
        QJsonArray bid = bidsArray[i].toArray();
        QJsonArray ask = asksArray[i].toArray();
        Ask _ask = {
            .askPrice = ask[0].toString().toDouble(),
            .askSize  = ask[1].toString().toDouble()
        };
        Bid _bid = {
            .bidPrice = bid[0].toString().toDouble(),
            .bidSize  = bid[1].toString().toDouble(),
        };
        order_books.totalAskVolume += _ask.askSize;
        order_books.totalBidVolume += _bid.bidSize;
        order_books.asks.append(_ask);
        order_books.bids.append(_bid);
    }

    order_books.spread = order_books.asks[0].askPrice - order_books.bids[0].bidPrice;

    emit updatedOrderBooks(order_books);
}

void OKXParser::sendSubscriptionMessage(const QStringList &streams) {
    for (int i = 0; i < streams.size(); i += MAX_STREAMS_PER_SUBSCRIPTION) {
        QJsonArray chunk;
        for (int j = i; j < qMin(i + MAX_STREAMS_PER_SUBSCRIPTION, streams.size()); ++j)
            chunk.append(QJsonDocument::fromJson(streams[j].toUtf8()).object());

        QJsonObject subscribeMessage;
        subscribeMessage["op"] = "subscribe";
        subscribeMessage["args"] = chunk;
        
        QJsonDocument doc(subscribeMessage);
        QString message = doc.toJson(QJsonDocument::Compact);

        webSocket->sendTextMessage(message);

        if (i + MAX_STREAMS_PER_SUBSCRIPTION < streams.size())
            QThread::msleep(100);
    }
}

void OKXParser::sendUnsubscriptionMessage(const QStringList &streams) {
    for (int i = 0; i < streams.size(); i += MAX_STREAMS_PER_SUBSCRIPTION) {
        QJsonArray chunk;
        for (int j = i; j < qMin(i + MAX_STREAMS_PER_SUBSCRIPTION, streams.size()); ++j)
            chunk.append(QJsonDocument::fromJson(streams[j].toUtf8()).object());
        
        QJsonObject unsubscribeMessage;
        unsubscribeMessage["op"] = "unsubscribe";
        unsubscribeMessage["args"] = chunk;
        
        QJsonDocument doc(unsubscribeMessage);
        QString message = doc.toJson(QJsonDocument::Compact);
        
        webSocket->sendTextMessage(message);
        
        if (i + MAX_STREAMS_PER_SUBSCRIPTION < streams.size())
            QThread::msleep(100);
    }
}

QString OKXParser::tickerStream(const QString &coin) {
    QString _coin = coin;
    QString okxSymbol = (t_market == TMarketData::SPOT) ? _coin.replace("/", "-") : QString("%1-SWAP").arg(_coin.replace("/", "-").toUpper());
    QJsonObject obj = {
        {"channel", "tickers"},
        {"instId", okxSymbol}
    };
    return QJsonDocument(obj).toJson(QJsonDocument::Compact);
}

QString OKXParser::orderBooksStream(const QString &coin) {
    QString _coin = coin;
    QString okxSymbol = (t_market == TMarketData::SPOT) ? _coin.replace("/", "-") : QString("%1-SWAP").arg(_coin.replace("/", "-").toUpper());
    QJsonObject obj = {
        {"channel", "books"},
        {"instId", okxSymbol}
    };
    return QJsonDocument(obj).toJson(QJsonDocument::Compact);
}
