#pragma once

#include "Configs/ScannerConfig.hpp"

#include "Parser/WebSocketParser.hpp"

class Scanner : public QObject {
    Q_OBJECT
private:

    ParamsScannerConfig pScannerConfig;

    QMap<QString, std::shared_ptr<WebSocketParser>> lstParsers;

private slots:

    void updateCoin(const QString& symbol, const WebSocketParser::stInfoCoin& _info);

public:

    QStringList getListStockMarket();

    void setScannerConfig(const ParamsScannerConfig& _config);
    ParamsScannerConfig getScannerConfig();

    void addStockMarket(const QString& StockMarket, const QString& Market, const QString& channel);
    void delStockMarket(const QString& StockMarket, const QString& Market);

    void start();
    void stop();

signals:

    void update_coin(const QString& symbol, const WebSocketParser::stInfoCoin& _info);

};
