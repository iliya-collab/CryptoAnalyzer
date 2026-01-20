#include "Parser/BybitParser.hpp"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <optional>

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
            updateBooks(obj);
    }
}

void BybitParser::updateTicker(const QJsonObject &json) {
    //qDebug() << QJsonDocument(json).toJson(QJsonDocument::Compact);
    if (!json.contains("data") || !json["data"].isObject())
        return;
        
    QJsonObject data = json["data"].toObject();
    QString coin = formatCoin(data["symbol"].toString());

    stInfoCoin _info;
    if (currentInfoAboutCoins.contains(coin))
        _info = currentInfoAboutCoins[coin];
    else
        _info = {};

    quint64 cur_ts = 0;
    bool needUpdate = false;

    if (json.contains("ts"))
        cur_ts = json["ts"].toInteger();
    if (cur_ts - _info.ts > MIN_CHANGE_TIME) {
        _info.ts = cur_ts;
        needUpdate = true;
    }

    if (data.contains("lastPrice"))
        _info.stPrice.curPrice = data["lastPrice"].toString().toDouble();

    if (data.contains("ask1Price") && data.contains("ask1Size") && data.contains("bid1Price") && data.contains("bid1Size")) {
        _info.stBooks.clear();
        stInfoBooks st_books;
        st_books = {
            .bidPrice = data["bid1Price"].toString().toDouble(),
            .askPrice = data["ask1Price"].toString().toDouble(),
            .bidSize  = data["bid1Size"].toString().toDouble(),
            .askSize  = data["ask1Size"].toString().toDouble(),
            .spread   = st_books.askPrice - st_books.bidPrice
        };
        _info.stBooks.append(st_books);
    }
    
    if (data.contains("highPrice24h") && data.contains("lowPrice24h") && data.contains("turnover24h") && data.contains("volume24h")) {
        _info.st24hStat.high24h = data["highPrice24h"].toString().toDouble();
        _info.st24hStat.low24h = data["lowPrice24h"].toString().toDouble();
        _info.st24hStat.volCcy24h = data["turnover24h"].toString().toDouble();
        _info.st24hStat.vol24h = data["volume24h"].toString().toDouble();
    }

    QWriteLocker locker(&dataLock);
    if (currentInfoAboutCoins.contains(coin)) {
        _info.stPrice.prevPrice = currentInfoAboutCoins[coin].stPrice.curPrice;
        _info.stPrice.difPrice = _info.stPrice.curPrice - _info.stPrice.prevPrice;
    }
    currentInfoAboutCoins[coin] = _info;
    locker.unlock();

    if (needUpdate || qAbs(_info.stPrice.difPrice / _info.stPrice.curPrice) > MIN_PRICE_CHANGE)
        emit updated(QString("%1:%2").arg(getNameMarket()).arg(coin), _info);
}

void BybitParser::updateBooks(const QJsonObject &json) {
    if (!json.contains("data") || !json["data"].isObject())
        return;
        
    QJsonObject data = json["data"].toObject();
    QString coin = formatCoin(data["symbol"].toString());

    stInfoCoin _info;
    if (currentInfoAboutCoins.contains(coin))
        _info = currentInfoAboutCoins[coin];
    else
        _info = {};

    QJsonArray asksArray = data["a"].toArray();
    QJsonArray bidsArray = data["b"].toArray();
    for (int i = 0; i < bidsArray.size() && i < mxDepthBooks; i++) {
        QJsonArray bid = bidsArray[i].toArray();
        QJsonArray ask = asksArray[i].toArray();
        _info.stBooks.clear();
        stInfoBooks st_books;
        st_books = {
            .bidPrice = bid[0].toString().toDouble(),
            .askPrice = ask[0].toString().toDouble(),
            .bidSize  = bid[1].toString().toDouble(),
            .askSize  = ask[1].toString().toDouble(),
            .spread   = st_books.askPrice - st_books.bidPrice
        };
        _info.stBooks.append(st_books);
    }

    QWriteLocker locker(&dataLock);
    if (currentInfoAboutCoins.contains(coin)) {
        _info.stPrice.prevPrice = currentInfoAboutCoins[coin].stPrice.curPrice;
        _info.stPrice.difPrice = _info.stPrice.curPrice - _info.stPrice.prevPrice;
    }
    currentInfoAboutCoins[coin] = _info;
    locker.unlock();

    emit updated(QString("%1:%2").arg(getNameMarket()).arg(coin), _info);
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

QString BybitParser::booksStream(const QString &coin) {
    QString _coin = coin;
    return QString("orderbook.%1").arg(_coin.replace("/", "").toUpper());
}