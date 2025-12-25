#pragma once

#include "Parser/WebSocketParser.hpp"

class BinanceParser : public WebSocketParser {
    Q_OBJECT

private:

    void sendSubscriptionMessage() override;
    void sendUnsubscriptionMessage(const QStringList &streams) override;
    void processPriceUpdate(const QJsonObject &json) override;
    QString coinToStream(const QString &coin) override;

public:
    explicit BinanceParser(QObject *parent = nullptr) : WebSocketParser(parent) {
        TradeUrl = QUrl("wss://stream.binance.com:9443/ws");
    };
    ~BinanceParser() {};

protected slots:

    void onTextMessageReceived(const QString &message) override;
    
};