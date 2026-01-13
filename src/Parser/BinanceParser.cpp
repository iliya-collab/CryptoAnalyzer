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
    else if (json.contains("e") && json["e"].toString() == "24hrTicker")
        processPriceUpdate(json);
    else if (json.contains("ping")) {
        QJsonObject pong{{"pong", json["ping"]}};
        webSocket->sendTextMessage(QJsonDocument(pong).toJson());
    }
}

void BinanceParser::processPriceUpdate(const QJsonObject &json) {
    if (!json.contains("s") || !json.contains("c"))
        return;

    QString coin = formatCoin(json["s"].toString());
    bool ok;
    double price = json["c"].toString().toDouble(&ok);


    if (ok && price > 0) {
        //qDebug() << "Binance: Received price for" << coin << ":" << price;
        QWriteLocker locker(&dataLock);

        auto& curCoin = currentInfoAboutCoins[coin];
        double oldPrice = curCoin.value;
        curCoin.value = price;
        curCoin.dif = price - oldPrice;

        locker.unlock();

        if (oldPrice == 0 || qAbs((price - oldPrice) / oldPrice) > MIN_PRICE_CHANGE)
            emit priceUpdated(coin, price);

    }
}

void BinanceParser::sendSubscriptionMessage() {
    if (subscribedCoins.isEmpty())
        return;

    QStringList streams;
    QReadLocker locker(&dataLock);
    for (QString coin : subscribedCoins)
        streams.append(coinToStream(coin));
    locker.unlock();

    for (int i = 0; i < streams.size(); i += MAX_STREAMS_PER_SUBSCRIPTION) {
        QStringList chunk = streams.mid(i, MAX_STREAMS_PER_SUBSCRIPTION);

        QJsonObject subscribeMessage;
        subscribeMessage["method"] = "SUBSCRIBE";
        subscribeMessage["params"] = QJsonArray::fromStringList(chunk);
        subscribeMessage["id"] = i + 1;

        QJsonDocument doc(subscribeMessage);
        QString message = doc.toJson(QJsonDocument::Compact);

        webSocket->sendTextMessage(message);

        // ЗАДЕРЖКА МЕЖДУ ПОДПИСКАМИ
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

QString BinanceParser::coinToStream(QString &coin) {
    return QString("%1@ticker").arg(coin.replace("/", "").toLower());
}
