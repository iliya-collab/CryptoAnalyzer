#pragma once

#include "Parser/ParserMyWallet.hpp"

#include "Configs/ScannerConfig.hpp"
#include "Configs/MyWalletConfig.hpp"

class Scanner : public QObject {
    Q_OBJECT
private:

    ParamsScannerConfig pScannerConfig;
    ParamsMyWalletConfig pMyWalletConfig;

    QMap<QString, std::shared_ptr<WebSocketParser>> lstParsers;

    std::unique_ptr<ParserMyWallet> myWalletParser;

private slots:

    void updateCoin(const QString& symbol, const WebSocketParser::stInfoCoin& _info);

public:

    explicit Scanner(QWidget *parent = nullptr);

    void setConfig(const ParamsScannerConfig& _ScannerConfig, const ParamsMyWalletConfig& _MyWalletConfig);

    QStringList getListStockMarket();
    ParamsScannerConfig getScannerConfig() {
        return pScannerConfig;
    }

    void addStockMarket(const QString& StockMarket, const QString& Market, const QString& channel);

    void start();
    void stop();

signals:

    void update_coin(const QString& symbol, const WebSocketParser::stInfoCoin& _info);

};
