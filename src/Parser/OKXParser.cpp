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

    stInfoCoin _info;

    if (ticker.contains("last")) {
        _info.stPrice.prevPrice = _info.stPrice.curPrice;
        _info.stPrice.curPrice = ticker["last"].toString().toDouble();
        _info.stPrice.difPrice = _info.stPrice.curPrice - _info.stPrice.prevPrice;
    }

    if (ticker.contains("askPx") && ticker.contains("askSz") && 
        ticker.contains("bidPx") && ticker.contains("bidSz")) {
        _info.stBooks[0].askPrice = ticker["askPx"].toString().toDouble();
        _info.stBooks[0].askSize = ticker["askSz"].toString().toDouble();
        _info.stBooks[0].bidPrice = ticker["bidPx"].toString().toDouble();
        _info.stBooks[0].askSize = ticker["bidSz"].toString().toDouble();
    }
    
    if (ticker.contains("high24h") && ticker.contains("low24h") && 
        ticker.contains("volCcy24h") && ticker.contains("vol24h")) {
        _info.st24hStat.high24h = ticker["high24h"].toString().toDouble();
        _info.st24hStat.low24h = ticker["low24h"].toString().toDouble();
        _info.st24hStat.volCсy24h = ticker["volCcy24h"].toString().toDouble();
        _info.st24hStat.vol24h = ticker["vol24h"].toString().toDouble();
    }

    // qDebug() << "OKX: Received price for" << coin << ":" << price << "(valid:" << ok << ")";

    QWriteLocker locker(&dataLock);
    currentInfoAboutCoins[coin] = _info;
    locker.unlock();

    if (_info.stPrice.prevPrice != 0 || qAbs((_info.stPrice.difPrice) / _info.stPrice.curPrice) > MIN_PRICE_CHANGE)
        emit priceUpdated(coin, _info.stPrice.curPrice);
}

void OKXParser::sendSubscriptionMessage(const QStringList &streams)
{
    if (streams.isEmpty())
        return;

    /*QReadLocker locker(&dataLock);

    QJsonArray args;

    for (QString coin : subscribedCoins) {
        QString stream = tickerStream(coin);

        QJsonObject arg = {
            {"channel", "tickers"},
            {"instId", stream}
        };
        args.append(arg);
    }

    locker.unlock();*/

    // Отправляем подписки порциями
    for (int i = 0; i < streams.size(); i += MAX_STREAMS_PER_SUBSCRIPTION) {

        QJsonArray chunk;
        for (int j = i; j < qMin(i + MAX_STREAMS_PER_SUBSCRIPTION, streams.size()); ++j)
            chunk.append(streams[j]);

        QJsonObject subscribeMessage{
            {"op", "subscribe"},
            {"args", chunk}
        };

        QJsonDocument doc(subscribeMessage);
        QString message = doc.toJson(QJsonDocument::Compact);

        webSocket->sendTextMessage(message);

        if (i + MAX_STREAMS_PER_SUBSCRIPTION < streams.size())
            QThread::msleep(100);
    }
}

void OKXParser::sendUnsubscriptionMessage(const QStringList &streams)
{
    if (streams.isEmpty())
        return;
    
    QJsonArray args;
    
    for (QString coin : streams) {
        QString stream = tickerStream(coin);
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

QString OKXParser::tickerStream(QString &coin) {
    // Формат: BTC/USDT -> "BTC-USDT" или "BTC-USDT-SWAP"
    QString okxSymbol = coin.replace("/", "-");
    return (t_market == TMarketData::SPOT) ? okxSymbol : QString("%1-SWAP").arg(okxSymbol.toUpper());
}

QString OKXParser::books1Stream(QString &coin) {
}

QString OKXParser::books5Stream(QString &coin) {
}
