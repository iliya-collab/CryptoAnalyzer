#include "Parser/BybitParser.hpp"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <optional>

BybitParser::BybitParser(const QString& name, TMarketData TMarket, QObject* parent) : WebSocketParser(name, TMarket, parent) {
    auto ret = getURLMarketData(TMarket);
    if (ret.has_value())
        Url = ret.value();
};

std::optional<QUrl> BybitParser::getURLMarketData(TMarketData TMarket) {
    switch (TMarket) {
        case TMarketData::SPOT:
            return QUrl("wss://stream.bybit.com/v5/public/spot");
        case TMarketData::FUTURES:
            return QUrl("wss://stream.bybit.com/v5/public/linear");
        default:
            return std::nullopt;
    }
}

QString BybitParser::formatCoin(const QString& coin) {
    return QString("%1:%2").arg(nameMarket).arg(coin);
}

void BybitParser::onTextMessageReceived(const QString &message) {
     //qDebug() << "Bybit RAW received:" << message;

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8(), &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "Bybit: JSON parse error:" << parseError.errorString();
        qDebug() << "Message:" << message;
        return;
    }

    if (!doc.isObject()) {
        qDebug() << "Bybit: Document is not an object";
        return;
    }

    QJsonObject json = doc.object();

    if (json.contains("success") && json.contains("op")) {
        QString operation = json["op"].toString();
        bool success = json["success"].toBool();
        if (!success) {
            QString retMsg = json["ret_msg"].toString();
            qWarning() << "Bybit" << operation << "failed:" << retMsg;
            emit errorOccurred(retMsg);
        }
        return;
    }
    else if (json.contains("topic") && json["topic"].toString().startsWith("tickers.")) {
        processPriceUpdate(json);
    }
    else if (json.contains("ping")) {
        QJsonObject pong{{"pong", json["ping"]}};
        webSocket->sendTextMessage(QJsonDocument(pong).toJson());
    }
}

void BybitParser::processPriceUpdate(const QJsonObject &json) {

    if (!json.contains("topic"))
        return;

    if (!json.contains("data") || !json["data"].isObject())
        return;
        
    QString coin = formatCoin(json["topic"].toString().mid(8));
    QJsonObject data = json["data"].toObject();

    stInfoCoin _info;

    if (data.contains("lastPrice")) {
        _info.stPrice.prevPrice = _info.stPrice.curPrice;
        _info.stPrice.curPrice = json["lastPrice"].toString().toDouble();
        _info.stPrice.difPrice = _info.stPrice.curPrice - _info.stPrice.prevPrice;
    }

    if (json.contains("ask1Price") && json.contains("ask1Size") && 
        json.contains("bid1Price") && json.contains("bid1Size")) {
        _info.stBooks[0].askPrice = json["ask1Price"].toString().toDouble();
        _info.stBooks[0].askSize = json["ask1Size"].toString().toDouble();
        _info.stBooks[0].bidPrice = json["bid1Price"].toString().toDouble();
        _info.stBooks[0].askSize = json["bid1Size"].toString().toDouble();
    }
    
    if (json.contains("highPrice24h") && json.contains("lowPrice24h") && 
        json.contains("turnover24h") && json.contains("volume24h")) {
        _info.st24hStat.high24h = json["highPrice24h"].toString().toDouble();
        _info.st24hStat.low24h = json["lowPrice24h"].toString().toDouble();
        _info.st24hStat.volCсy24h = json["turnover24h"].toString().toDouble();
        _info.st24hStat.vol24h = json["volume24h"].toString().toDouble();
    }

        
    QWriteLocker locker(&dataLock);
    currentInfoAboutCoins[coin] = _info;
    locker.unlock();

    if (_info.stPrice.prevPrice != 0 || qAbs((_info.stPrice.difPrice) / _info.stPrice.curPrice) > MIN_PRICE_CHANGE)
        emit priceUpdated(coin, _info.stPrice.curPrice);
}

void BybitParser::sendSubscriptionMessage()
{
    if (subscribedCoins.isEmpty())
        return;

    QReadLocker locker(&dataLock);
    QStringList streams;
    for (QString coin : subscribedCoins)
        streams.append(tickerStream(coin));
    locker.unlock();

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

void BybitParser::sendUnsubscriptionMessage(const QStringList &streams)
{
    if (streams.isEmpty())
        return;

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

QString BybitParser::tickerStream(QString &coin) {
    return QString("tickers.%1").arg(coin.replace("/", "").toUpper());
}

QString BybitParser::books1Stream(QString &coin) {
}

QString BybitParser::books5Stream(QString &coin) {
}
