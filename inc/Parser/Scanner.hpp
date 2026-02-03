#pragma once

#include "Configs/PlatformConfig.hpp"

/*#include "Parser/WebSocketParser.hpp"
#include "Parser/RegisterParsers.hpp"

class Scanner : public QObject {
    Q_OBJECT
private:

    ParamsPlatformConfig m_platform_config;

    QMap<QString, std::shared_ptr<WebSocketParser>> lstParsers;

    void setupParserConnections(WebSocketParser* parser, const QSet<QString>& channels);

private slots:

    void updateTicker(const WebSocketParser::stTicker& _ticker);
    void updateOrderBooks(const WebSocketParser::stOrderBooks& _orderBooks);

public:

    QStringList getListStockMarket();

    void setConfig(const ParamsPlatformConfig& _config);
    ParamsPlatformConfig getConfig();

    void addStockMarket(const QString& StockMarket);
    void delStockMarket(const QString& StockMarket);

    void addChannels(const QString& StockMarket, const QSet<QString>& channels);

    void start();
    void stop();

signals:

    void ticker(const WebSocketParser::stTicker& _ticker);
    void orderBooks(const WebSocketParser::stOrderBooks& _orderBooks);

};*/
