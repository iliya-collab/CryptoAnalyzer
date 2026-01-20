#pragma once

#include "Parser/WebSocketParser.hpp"

class BinanceParser : public WebSocketParser {
    Q_OBJECT

private:

    void sendSubscriptionMessage(const QStringList &streams) override;
    void sendUnsubscriptionMessage(const QStringList &streams) override;
    void messageReceived(const QJsonObject &obj) override;
    
    void updateTicker(const QJsonObject &json) override;
    void updateBooks(const QJsonObject &json) override;
    
    QString tickerStream(const QString &coin) override;
    QString booksStream(const QString &coin) override;

    std::expected<QUrl, QString> getURLMarketData() override;

    QString formatCoin(const QString& coin) override;

public:

    explicit BinanceParser(const QString& name, QObject *parent = nullptr) : WebSocketParser(name, parent) {};
    ~BinanceParser() {};

};
