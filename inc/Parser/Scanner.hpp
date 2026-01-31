#pragma once

#include "Configs/ScannerConfig.hpp"

#include "Parser/WebSocketParser.hpp"
#include "Parser/RegisterParsers.hpp"

class Scanner : public QObject {
    Q_OBJECT
private:

    ParamsScannerConfig pScannerConfig;

    QMap<QString, std::shared_ptr<WebSocketParser>> lstParsers;

    void setupParserConnections(WebSocketParser* parser, const QSet<QString>& channels);

private slots:

    void updateTicker(const WebSocketParser::stTicker& _ticker);
    void updateOrderBooks(const WebSocketParser::stOrderBooks& _orderBooks);

public:

    QStringList getListStockMarket();

    void setScannerConfig(const ParamsScannerConfig& _config);
    ParamsScannerConfig getScannerConfig();

    void addStockMarket(const QString& StockMarket);
    void delStockMarket(const QString& StockMarket);

    void addChannels(const QString& StockMarket, const QSet<QString>& channels);

    void start();
    void stop();

signals:

    void ticker(const WebSocketParser::stTicker& _ticker);
    void orderBooks(const WebSocketParser::stOrderBooks& _orderBooks);

};
