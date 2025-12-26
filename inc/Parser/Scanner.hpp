#pragma once

#include "Parser/WebSocketParser.hpp"

#include "Configs/ScannerConfig.hpp"
#include "Configs/MyWalletConfig.hpp"

class Scanner : public QObject {
    Q_OBJECT

private:

    ParamsScannerConfig ScannerConfig;
    ParamsMyWalletConfig MyWalletConfig;

    std::unique_ptr<WebSocketParser> wbParser;

    std::unique_ptr<QTimer> autoDisconnectTimer;
    std::unique_ptr<QTimer> triggeredRuleTimer;

    void initTimers();

public:

    explicit Scanner(WebSocketParser* _wsParser, const ParamsScannerConfig& _ScannerConfig, const ParamsMyWalletConfig& _MyWalletConfig);

};