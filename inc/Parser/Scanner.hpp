#pragma once

#include "Parser/WebSocketParser.hpp"
#include "Parser/ParserMyWallet.hpp"

#include "CustomWindowDialogs/StatusPanel.hpp"

#include "Configs/ScannerConfig.hpp"
#include "Configs/MyWalletConfig.hpp"

class Scanner : public QObject {
    Q_OBJECT

private:

    std::unique_ptr<StatusPanel> statPanel;

    ParamsScannerConfig pScannerConfig;
    ParamsMyWalletConfig pMyWalletConfig;

    std::unique_ptr<WebSocketParser> wsParser;
    std::unique_ptr<ParserMyWallet> myWalletParser;

    std::unique_ptr<QTimer> autoDisconnectTimer;
    std::unique_ptr<QTimer> triggeredRuleTimer;

    void initTimers();

private slots:

    void onDisconnectTimeout();
    void onTriggeredRuleTimer();

    // Подключаются к WebSocketParser
    // -------------------------------------------------
    // Вызывается при обновлении цены на подписанные монеты
    void onPriceUpdated(const QString &coin, double price);
    // Вызывается при подключении к websocket к серверу
    void onWebSocketConnected();
    // Вызывается при отключении websocket от серверу
    void onWebSocketDisconnected();
    // Вызывается при возникновении ошибок в подключении
    void onWebSocketError(const QString &error);
    // -------------------------------------------------

public:

    explicit Scanner(QWidget *parent = nullptr);

    void setWebSocketParser(std::unique_ptr<WebSocketParser> _wsParser);
    void setConfig(const ParamsScannerConfig& _ScannerConfig, const ParamsMyWalletConfig& _MyWalletConfig);

    void start();
    void stop();

};