#pragma once

#include "Parser/ParserMyWallet.hpp"

#include "Configs/ScannerConfig.hpp"
#include "Configs/MyWalletConfig.hpp"

#include "CustomWindowDialogs/StatusPanel.hpp"

class Scanner : public QObject {
    Q_OBJECT

private:

    ParamsScannerConfig pScannerConfig;
    ParamsMyWalletConfig pMyWalletConfig;

    std::unique_ptr<StatusPanel> statPanel;

    QMap<QString, std::shared_ptr<WebSocketParser>> lstParsers;

    std::unique_ptr<ParserMyWallet> myWalletParser;

    void connectSignals();
    void initParsers();

private slots:

    // Вызывается при обновлении цены на подписанные монеты
    void updateTable(const QString &coin, double price);
    void onStartConnection();
    void onDialogTable();
    void onDialogGraph();

public:

    explicit Scanner(QWidget *parent = nullptr);

    void setConfig(const ParamsScannerConfig& _ScannerConfig, const ParamsMyWalletConfig& _MyWalletConfig);

    void start();
    void stop();

};
