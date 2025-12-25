#include "Parser/BinanceParser.hpp"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

void BinanceParser::onTextMessageReceived(const QString &message) {
    // ОБРАБОТКА ВХОДЯЩИХ СООБЩЕНИЙ
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8(), &parseError);
    
    if (parseError.error != QJsonParseError::NoError)
        return;

    if (!doc.isObject())
        return;

    QJsonObject json = doc.object();
    
    // ОБРАБОТКА РАЗНЫХ ТИПОВ СООБЩЕНИЙ
    if (json.contains("result") && json.contains("id")) {
        // Ответ на подписку - игнорируем, Binance может не отправлять подтверждение
        return;
    } 
    else if (json.contains("e") && json["e"].toString() == "24hrTicker") {
        // Обновление цены
        processPriceUpdate(json);
    }
    else if (json.contains("ping")) {
        // Ответ на ping - отправляем pong
        QJsonObject pong{{"pong", json["ping"]}};
        webSocket->sendTextMessage(QJsonDocument(pong).toJson());
    }
}

void BinanceParser::processPriceUpdate(const QJsonObject &json) {
    if (!json.contains("s") || !json.contains("c"))
        return;

    QString symbol = json["s"].toString();
    QString priceStr = json["c"].toString();
    
    // Извлекаем название монеты (убираем USDT)
    QString coin = symbol.left(symbol.length() - 4).toUpper();
    
    bool ok;
    double price = priceStr.toDouble(&ok);
    
    if (ok && price > 0) {

        QWriteLocker locker(&dataLock);

        auto& curCoin = currentInfoAboutCoins[coin];
        double oldPrice = curCoin.value;
        curCoin = {price, price-oldPrice};

        locker.unlock();
        
        // ЭМИТИМ ТОЛЬКО ЕСЛИ ЦЕНА ИЗМЕНИЛАСЬ
        if (qAbs(curCoin.dif) > 0.001)
            emit priceUpdated(coin, price);

    }
}

void BinanceParser::sendSubscriptionMessage() {
    if (subscribedCoins.isEmpty())
        return;

    // СОЗДАЕМ СПИСОК STREAMS
    QStringList streams;
    QReadLocker locker(&dataLock);
    for (const QString &coin : subscribedCoins)
        streams.append(coinToStream(coin));
    locker.unlock();

    // ОГРАНИЧИВАЕМ КОЛИЧЕСТВО STREAMS В ОДНОЙ ПОДПИСКЕ
    const int MAX_STREAMS_PER_SUBSCRIPTION = 10;
    
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

    // ОГРАНИЧИВАЕМ КОЛИЧЕСТВО STREAMS В ОДНОЙ ОТПИСКЕ
    const int MAX_STREAMS_PER_UNSUBSCRIPTION = 10;
    
    for (int i = 0; i < streams.size(); i += MAX_STREAMS_PER_UNSUBSCRIPTION) {
        QStringList chunk = streams.mid(i, MAX_STREAMS_PER_UNSUBSCRIPTION);
        
        QJsonObject unsubscribeMessage;
        unsubscribeMessage["method"] = "UNSUBSCRIBE";
        unsubscribeMessage["params"] = QJsonArray::fromStringList(chunk);
        unsubscribeMessage["id"] = 1000 + i; // ID для отписок

        QJsonDocument doc(unsubscribeMessage);
        QString message = doc.toJson(QJsonDocument::Compact);
        
        webSocket->sendTextMessage(message);
        
        // ЗАДЕРЖКА МЕЖДУ ОТПИСКАМИ
        if (i + MAX_STREAMS_PER_UNSUBSCRIPTION < streams.size()) 
            QThread::msleep(100);
    }
}

QString BinanceParser::coinToStream(const QString &coin) {
    return QString("%1@ticker").arg(coin.toLower());
}