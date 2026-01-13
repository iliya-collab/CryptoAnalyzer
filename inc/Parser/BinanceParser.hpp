#pragma once

#include "Parser/WebSocketParser.hpp"

class BinanceParser : public WebSocketParser {
    Q_OBJECT

private:

    void sendSubscriptionMessage(const QStringList &streams) override;
    void sendUnsubscriptionMessage(const QStringList &streams) override;
    
    void updateTicker(const QJsonObject &json) override;
    void updateBooks1(const QJsonObject &json) override;
    void updateBooks5(const QJsonObject &json) override;
    
    QString tickerStream(QString &coin) override;
    QString books1Stream(QString &coin) override;
    QString books5Stream(QString &coin) override;


    std::optional<QUrl> getURLMarketData(TMarketData TMarket) override;

    QString formatCoin(const QString& coin) override;

public:

    explicit BinanceParser(const QString& name, TMarketData TMarket, QObject *parent = nullptr);
    ~BinanceParser() {};

protected slots:

    void onTextMessageReceived(const QString &message) override;

};
