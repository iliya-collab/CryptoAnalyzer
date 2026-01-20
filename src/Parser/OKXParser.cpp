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
            updateBooks(obj);
    }
}

void OKXParser::updateTicker(const QJsonObject &json) {
    QJsonObject arg = json["arg"].toObject();
    QString coin = formatCoin(arg["instId"].toString());

    QJsonArray dataArray = json["data"].toArray();
    if (dataArray.isEmpty())
        return;

    QJsonObject ticker = dataArray[0].toObject();

    stInfoCoin _info;
    if (currentInfoAboutCoins.contains(coin))
        _info = currentInfoAboutCoins[coin];
    else
        _info = {};

    quint64 cur_ts = 0;
    bool needUpdate = false;

    if (ticker.contains("ts"))
        cur_ts = ticker["ts"].toInteger();
    if (cur_ts - _info.ts > MIN_CHANGE_TIME) {
        _info.ts = cur_ts;
        needUpdate = true;
    }

    if (ticker.contains("last"))
        _info.stPrice.curPrice = ticker["last"].toString().toDouble();

    if (ticker.contains("askPx") && ticker.contains("askSz") && ticker.contains("bidPx") && ticker.contains("bidSz")) {
        _info.stBooks.clear();
        stInfoBooks st_books;
        st_books = {
            .bidPrice = ticker["bidPx"].toString().toDouble(),
            .askPrice = ticker["askPx"].toString().toDouble(),
            .bidSize  = ticker["bidSz"].toString().toDouble(),
            .askSize  = ticker["askSz"].toString().toDouble(),
            .spread   = st_books.askPrice - st_books.bidPrice
        };
        _info.stBooks.append(st_books);
    }
    
    if (ticker.contains("high24h") && ticker.contains("low24h") && ticker.contains("volCcy24h") && ticker.contains("vol24h")) {
        _info.st24hStat.high24h = ticker["high24h"].toString().toDouble();
        _info.st24hStat.low24h = ticker["low24h"].toString().toDouble();
        _info.st24hStat.volCcy24h = ticker["volCcy24h"].toString().toDouble();
        _info.st24hStat.vol24h = ticker["vol24h"].toString().toDouble();
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

void OKXParser::updateBooks(const QJsonObject &json) {
    QJsonObject arg = json["arg"].toObject();
    QString coin = formatCoin(arg["instId"].toString());

    QJsonArray dataArray = json["data"].toArray();
    if (dataArray.isEmpty())
        return;

    QJsonObject books = dataArray[0].toObject();

    stInfoCoin _info;
    if (currentInfoAboutCoins.contains(coin)) 
        _info = currentInfoAboutCoins[coin];
    else _info = {};

    _info.stBooks.clear();

    QJsonArray asksArray = books["asks"].toArray();
    QJsonArray bidsArray = books["bids"].toArray();
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
    currentInfoAboutCoins[coin] = _info;
    locker.unlock();

    emit updated(QString("%1:%2").arg(getNameMarket()).arg(coin), _info);
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

QString OKXParser::booksStream(const QString &coin) {
    QString _coin = coin;
    QString okxSymbol = (t_market == TMarketData::SPOT) ? _coin.replace("/", "-") : QString("%1-SWAP").arg(_coin.replace("/", "-").toUpper());
    QJsonObject obj = {
        {"channel", "books"},
        {"instId", okxSymbol}
    };
    return QJsonDocument(obj).toJson(QJsonDocument::Compact);
}
