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
    }
    else if (obj.contains("bids") && obj.contains("asks"))
        updateBooks20(obj);

}

void BinanceParser::updateTicker(const QJsonObject &json) {
    if (!json.contains("s"))
        return;

    QString coin = json["s"].toString();

    static quint64 ts = 0;
    quint64 cur_ts = 0;
    bool needUpdate = false;

    if (json.contains("E"))
        cur_ts = json["E"].toInteger();
    if (cur_ts - ts > MIN_CHANGE_TIME) {
        ts = cur_ts;
        needUpdate = true;
    }

    stInfoCoin _info;
    if (currentInfoAboutCoins.contains(coin))
        _info = currentInfoAboutCoins[coin];
    else
        _info = {};

    if (json.contains("c")) 
        _info.stPrice.curPrice = json["c"].toString().toDouble();

    if (json.contains("a") && json.contains("A") && 
        json.contains("b") && json.contains("B")) {
        _info.stBooks[0].askPrice = json["a"].toString().toDouble();
        _info.stBooks[0].askSize = json["A"].toString().toDouble();
        _info.stBooks[0].bidPrice = json["b"].toString().toDouble();
        _info.stBooks[0].askSize = json["B"].toString().toDouble();
    }
    
    if (json.contains("h") && json.contains("l") && 
        json.contains("q") && json.contains("v")) {
        _info.st24hStat.high24h = json["h"].toString().toDouble();
        _info.st24hStat.low24h = json["l"].toString().toDouble();
        _info.st24hStat.volCcy24h = json["q"].toString().toDouble();
        _info.st24hStat.vol24h = json["v"].toString().toDouble();
    }
    
    QWriteLocker locker(&dataLock);
     if (currentInfoAboutCoins.contains(coin)) {
        _info.stPrice.prevPrice = currentInfoAboutCoins[coin].stPrice.curPrice;
        _info.stPrice.difPrice = _info.stPrice.curPrice - _info.stPrice.prevPrice;
    }
    currentInfoAboutCoins[coin] = _info;
    locker.unlock();

    if (_info.stPrice.prevPrice == 0 || needUpdate || qAbs(_info.stPrice.difPrice / _info.stPrice.curPrice) > MIN_PRICE_CHANGE)
        emit updated(QString("%1:%2").arg(getNameMarket()).arg(coin), _info);

}

void BinanceParser::updateBooks5(const QJsonObject &json) {}

void BinanceParser::updateBooks10(const QJsonObject &json) {}

void BinanceParser::updateBooks20(const QJsonObject &json) {
    /*if (!json.contains("s"))
        return;

    QString coin = formatCoin(json["s"].toString());

    stInfoCoin _info;

    if (json.contains("a") && json.contains("b")) {
        QJsonArray asks = json["a"].toArray();
        QJsonArray bids = json["b"].toArray();
        _info.stBooks[0].askPrice = asks[0].toString().toDouble();
        _info.stBooks[0].askSize = asks[1].toString().toDouble();
        _info.stBooks[0].bidPrice = bids[0].toString().toDouble();
        _info.stBooks[0].askSize = bids[1].toString().toDouble();
    }

    QWriteLocker locker(&dataLock);
    currentInfoAboutCoins[coin] = _info;
    locker.unlock();
    emit updated(QString("%1:%2").arg(getNameMarket()).arg(coin), _info);*/
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

QString BinanceParser::books5Stream(const QString &coin) {
    QString _coin = coin;
    return QString("%1@depth5").arg(_coin.replace("/", "").toLower());
}

QString BinanceParser::books10Stream(const QString &coin) {
    QString _coin = coin;
    return QString("%1@depth10").arg(_coin.replace("/", "").toLower());
}

QString BinanceParser::books20Stream(const QString &coin) {
    QString _coin = coin;
    return QString("%1@depth20").arg(_coin.replace("/", "").toLower());
}

