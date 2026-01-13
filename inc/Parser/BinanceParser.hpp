#pragma once

#include "Parser/WebSocketParser.hpp"

class BinanceParser : public WebSocketParser {
    Q_OBJECT

private:

    void sendSubscriptionMessage() override;
    void sendUnsubscriptionMessage(const QStringList &streams) override;
    void processPriceUpdate(const QJsonObject &json) override;
    QString coinToStream(QString &coin) override;

    std::optional<QUrl> getURLMarketData(TMarketData TMarket) override;

    QString formatCoin(const QString& coin) override;

public:

    explicit BinanceParser(const QString& name, TMarketData TMarket, QObject *parent = nullptr);
    ~BinanceParser() {};

protected slots:

    void onTextMessageReceived(const QString &message) override;

};
