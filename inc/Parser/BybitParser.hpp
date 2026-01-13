#pragma once

#include "Parser/WebSocketParser.hpp"

class BybitParser : public WebSocketParser {
    Q_OBJECT

private:

    void sendSubscriptionMessage() override;
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

    explicit BybitParser(const QString& name, TMarketData TMarket, QObject *parent = nullptr);
    ~BybitParser() {};

protected slots:

    void onTextMessageReceived(const QString &message) override;

};
