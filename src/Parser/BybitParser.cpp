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

    QString symbol = json["topic"].toString().mid(8);

    if (!json.contains("data") || !json["data"].isObject())
        return;

    QJsonObject data = json["data"].toObject();

    if (!data.contains("lastPrice"))
        return;

    QString priceStr = data["lastPrice"].toString();

    QString coin = formatCoin(symbol);
    bool ok;
    double price = data["lastPrice"].toString().toDouble(&ok);

    if (ok && price > 0) {
        //qDebug() << "Bybit: Received price for" << symbol << ":" << price;
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

void BybitParser::sendSubscriptionMessage()
{
    if (subscribedCoins.isEmpty())
        return;

    QReadLocker locker(&dataLock);
    QStringList streams;
    for (QString coin : subscribedCoins)
        streams.append(coinToStream(coin));
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

QString BybitParser::coinToStream(QString &coin) {
    return QString("tickers.%1").arg(coin.replace("/", "").toUpper());
}
