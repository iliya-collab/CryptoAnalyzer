#pragma once

#include "Parser/WebSocketParser.hpp"

class BybitParser : public WebSocketParser {
    Q_OBJECT

private:

    void sendSubscriptionMessage(const QStringList &streams) override;
    void sendUnsubscriptionMessage(const QStringList &streams) override;
    void messageReceived(const QJsonObject &obj) override;
    
    void updateTicker(const QJsonObject &json) override;
    void updateBooks5(const QJsonObject &json) override;
    void updateBooks10(const QJsonObject &json) override;
    void updateBooks20(const QJsonObject &json) override;
    
    QString tickerStream(const QString &coin) override;
    QString books5Stream(const QString &coin) override;
    QString books10Stream(const QString &coin) override;
    QString books20Stream(const QString &coin) override;


    std::expected<QUrl, QString> getURLMarketData() override;

    QString formatCoin(const QString& coin) override;

public:

    explicit BybitParser(const QString& name, QObject *parent = nullptr) : WebSocketParser(name, parent) {};
    ~BybitParser() {};

};
