#include "Parser/OKXParser.hpp"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <optional>

OKXParser::OKXParser(const QString& name, TMarketData TMarket, QObject* parent) : WebSocketParser(name, TMarket, parent) {
    auto ret = getURLMarketData(TMarket);
    if (ret.has_value())
        Url = ret.value();
};

std::optional<QUrl> OKXParser::getURLMarketData(TMarketData TMarket) {
    t_market = TMarket;
    switch (TMarket) {
        case TMarketData::SPOT:
        case TMarketData::FUTURES:
            return QUrl("wss://ws.okx.com/ws/v5/public");
        default:
            return std::nullopt;
    }
}

QString OKXParser::formatCoin(const QString& coin) {
    QStringList lst = coin.split('-');
    return QString("%1:%2").arg(nameMarket).arg(lst[0]+lst[1]);
}

void OKXParser::onTextMessageReceived(const QString &message) {
    //qDebug() << "OKX RAW received:" << message;

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8(), &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "OKX: JSON parse error:" << parseError.errorString();
        qDebug() << "Message:" << message;
        return;
    }

    if (!doc.isObject()) {
        qDebug() << "OKX: Document is not an object";
        return;
    }

    QJsonObject json = doc.object();

    if (json.contains("arg") && json.contains("data")) {
        QJsonObject arg = json["arg"].toObject();
        QString channel = arg["channel"].toString();
        if (channel == "tickers")
            processPriceUpdate(json);
    }
    else if (json.contains("ping")) {
        QJsonObject pong{{"pong", json["ping"]}};
        webSocket->sendTextMessage(QJsonDocument(pong).toJson());
    }
}

void OKXParser::processPriceUpdate(const QJsonObject &json) {
    QJsonObject arg = json["arg"].toObject();
    // Например "BTC-USDT" или "BTC-USDT-SWAP" -> "BTCUSDT"
    QString coin = formatCoin(arg["instId"].toString());

    QJsonArray dataArray = json["data"].toArray();
    if (dataArray.isEmpty())
        return;

    QJsonObject ticker = dataArray[0].toObject();

    QString priceStr;
    if (ticker.contains("last"))
        priceStr = ticker["last"].toString();
    else {
        qDebug() << "OKX: No price field in ticker data";
        return;
    }

    bool ok;
    double price = priceStr.toDouble(&ok);

    // qDebug() << "OKX: Received price for" << coin << ":" << price << "(valid:" << ok << ")";

    if (ok && price > 0) {
        QWriteLocker locker(&dataLock);

        auto& curCoin = currentInfoAboutCoins[coin];
        double oldPrice = curCoin.value;
        curCoin.value = price;
        curCoin.dif = price - oldPrice;

        locker.unlock();

        if (oldPrice == 0 || qAbs((price - oldPrice) / oldPrice) > MIN_PRICE_CHANGE) {
            emit priceUpdated(coin, price);
        }
    }
}

void OKXParser::sendSubscriptionMessage()
{
    if (subscribedCoins.isEmpty())
        return;

    QReadLocker locker(&dataLock);

    QJsonArray args;

    for (QString coin : subscribedCoins) {
        QString stream = coinToStream(coin);

        QJsonObject arg = {
            {"channel", "tickers"},
            {"instId", stream}
        };
        args.append(arg);
    }

    locker.unlock();

    // Отправляем подписки порциями
    for (int i = 0; i < args.size(); i += MAX_STREAMS_PER_SUBSCRIPTION) {

        QJsonArray chunk;
        for (int j = i; j < qMin(i + MAX_STREAMS_PER_SUBSCRIPTION, args.size()); ++j)
            chunk.append(args[j]);

        QJsonObject subscribeMessage{
            {"op", "subscribe"},
            {"args", chunk}
        };

        QJsonDocument doc(subscribeMessage);
        QString message = doc.toJson(QJsonDocument::Compact);

        webSocket->sendTextMessage(message);

        if (i + MAX_STREAMS_PER_SUBSCRIPTION < args.size())
            QThread::msleep(100);
    }
}

void OKXParser::sendUnsubscriptionMessage(const QStringList &streams)
{
    if (streams.isEmpty())
        return;
    
    QJsonArray args;
    
    for (QString coin : streams) {
        QString stream = coinToStream(coin);
        qDebug() << stream << "unsubscribe";
        QJsonObject arg = {
            {"channel", "tickers"},
            {"instId", stream}
        };
        args.append(arg);
    }
    
    if (args.isEmpty())
        return;
    
    for (int i = 0; i < args.size(); i += MAX_STREAMS_PER_SUBSCRIPTION) {
        QJsonArray chunk;
        for (int j = i; j < qMin(i + MAX_STREAMS_PER_SUBSCRIPTION, args.size()); ++j)
            chunk.append(args[j]);
        
        QJsonObject unsubscribeMessage{
            {"op", "unsubscribe"},
            {"args", chunk}
        };
        
        QJsonDocument doc(unsubscribeMessage);
        QString message = doc.toJson(QJsonDocument::Compact);
        
        webSocket->sendTextMessage(message);
        
        if (i + MAX_STREAMS_PER_SUBSCRIPTION < args.size())
            QThread::msleep(100);
    }
}

QString OKXParser::coinToStream(QString &coin) {
    // Формат: BTC/USDT -> "BTC-USDT" или "BTC-USDT-SWAP"
    QString okxSymbol = coin.replace("/", "-");
    return (t_market == TMarketData::SPOT) ? okxSymbol : QString("%1-SWAP").arg(okxSymbol.toUpper());
}
