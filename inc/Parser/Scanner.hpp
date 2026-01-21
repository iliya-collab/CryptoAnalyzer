#pragma once

#include "Configs/ScannerConfig.hpp"

#include "Parser/WebSocketParser.hpp"

class Scanner : public QObject {
    Q_OBJECT
private:

    ParamsScannerConfig pScannerConfig;

    QMap<QString, std::shared_ptr<WebSocketParser>> lstParsers;

    void setupParserConnections(WebSocketParser* parser, const QString& channel);

private slots:

    void updateTicker(const WebSocketParser::stTicker& _ticker);
    void updateOrderBooks(const WebSocketParser::stOrderBooks& _orderBooks);

public:

    QStringList getListStockMarket();

    void setScannerConfig(const ParamsScannerConfig& _config);
    ParamsScannerConfig getScannerConfig();

    void addStockMarket(const QString& StockMarket, const QString& Market, const QString& channel);
    void delStockMarket(const QString& StockMarket, const QString& Market);

    void start();
    void stop();

signals:

    void ticker(const WebSocketParser::stTicker& _ticker);
    void orderBooks(const WebSocketParser::stOrderBooks& _orderBooks);

};
