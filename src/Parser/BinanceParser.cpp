#include "Parser/BinanceParser.hpp"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

BinanceParser::BinanceParser(const QString& name, TMarketData TMarket, QObject* parent) : WebSocketParser(name, TMarket, parent) {
    auto ret = getURLMarketData(TMarket);
    if (ret.has_value())
        Url = ret.value();
};

std::optional<QUrl> BinanceParser::getURLMarketData(TMarketData TMarket) {
    switch (TMarket) {
        case TMarketData::SPOT:
            return QUrl("wss://stream.binance.com/ws");
        case TMarketData::FUTURES:
            return QUrl("wss://fstream.binance.com/ws");
        default:
            return std::nullopt;
    }
}

QString BinanceParser::formatCoin(const QString& coin) {
    return QString("%1:%2").arg(nameMarket).arg(coin);
}

void BinanceParser::onTextMessageReceived(const QString &message) {
    //qDebug() << "Binance RAW received:" << message;

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8(), &parseError);

    if (parseError.error != QJsonParseError::NoError)
        return;

    if (!doc.isObject())
        return;

    QJsonObject json = doc.object();

    if (json.contains("result") && json.contains("id"))
        return;
    else if (json.contains("stream")) {
        QString stream = json["stream"].toString();
        if (stream.endsWith("@ticker"))
            updateTicker(json);
        else if (stream.endsWith("@depth") && !stream.endsWith("@depth5"))
            updateBooks1(json);
        else if (stream.endsWith("@depth5"))
            updateBooks5(json);

    }
    else if (json.contains("ping")) {
        QJsonObject pong{{"pong", json["ping"]}};
        webSocket->sendTextMessage(QJsonDocument(pong).toJson());
    }
}

void BinanceParser::updateTicker(const QJsonObject &json) {
    qDebug() << QString("%1:%2").arg(getNameMarket()).arg(json["stream"].toString());

    if (!json.contains("s"))
        return;

    QString coin = json["s"].toString();

    stInfoCoin _info;

    if (json.contains("c")) {
        _info.stPrice.prevPrice = _info.stPrice.curPrice;
        _info.stPrice.curPrice = json["c"].toString().toDouble();
        _info.stPrice.difPrice = _info.stPrice.curPrice - _info.stPrice.prevPrice;
    }

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
        _info.st24hStat.volCсy24h = json["q"].toString().toDouble();
        _info.st24hStat.vol24h = json["v"].toString().toDouble();
    }
    
    QWriteLocker locker(&dataLock);
    currentInfoAboutCoins[coin] = _info;
    locker.unlock();

    if (_info.stPrice.prevPrice != 0 || qAbs((_info.stPrice.difPrice) / _info.stPrice.curPrice) > MIN_PRICE_CHANGE)
        emit updated(formatCoin(coin), _info);

}

void BinanceParser::updateBooks1(const QJsonObject &json) {
    qDebug() << QString("%1:%2").arg(getNameMarket()).arg(json["stream"].toString());

    if (!json.contains("s"))
        return;

    QString coin = json["s"].toString();

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

    emit updated(formatCoin(coin), _info);

}

void BinanceParser::updateBooks5(const QJsonObject &json) {
    
    if (!json.contains("s"))
        return;
    
    qDebug() << QString("%1:%2").arg(getNameMarket()).arg(json["stream"].toString());
    
    QString coin = json["s"].toString();

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

    emit updated(formatCoin(coin), _info);
}

void BinanceParser::sendSubscriptionMessage(const QStringList &streams) {
    if (streams.isEmpty())
        return;

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
    if (streams.isEmpty())
        return;

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

QString BinanceParser::tickerStream(QString &coin) {
    return QString("%1@ticker").arg(coin.replace("/", "").toLower());
}

QString BinanceParser::books1Stream(QString &coin) {
    return QString("%1@depth").arg(coin.replace("/", "").toLower());
}

QString BinanceParser::books5Stream(QString &coin) {
    return QString("%1@depth5").arg(coin.replace("/", "").toLower());
}
