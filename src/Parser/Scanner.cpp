#include "Parser/Scanner.hpp"

Scanner::Scanner(QWidget *parent) {

    statPanel = std::make_unique<StatusPanel>(parent);
    myWalletParser = std::make_unique<ParserMyWallet>();

    initTimers();
}
    
void Scanner::setWebSocketParser(std::unique_ptr<WebSocketParser> _wsParser) {
    wsParser = std::move(_wsParser);

    if (wsParser) {
        connect(wsParser.get(), &WebSocketParser::priceUpdated, this, &Scanner::onPriceUpdated);
        connect(wsParser.get(), &WebSocketParser::connected, this, &Scanner::onWebSocketConnected);
        connect(wsParser.get(), &WebSocketParser::disconnected, this, &Scanner::onWebSocketDisconnected);
        connect(wsParser.get(), &WebSocketParser::errorOccurred, this, &Scanner::onWebSocketError);
    }
}

void Scanner::setConfig(const ParamsScannerConfig& _ScannerConfig, const ParamsMyWalletConfig& _MyWalletConfig) {
    pScannerConfig = _ScannerConfig;
    pMyWalletConfig = _MyWalletConfig;
}

void Scanner::initTimers() {

    autoDisconnectTimer = std::make_unique<QTimer>();
    autoDisconnectTimer->setSingleShot(true);
    connect(autoDisconnectTimer.get(), &QTimer::timeout, this, &Scanner::onDisconnectTimeout);

    triggeredRuleTimer = std::make_unique<QTimer>();
    triggeredRuleTimer->setSingleShot(true);
    connect(triggeredRuleTimer.get(), &QTimer::timeout, this, &Scanner::onTriggeredRuleTimer);
}

void Scanner::onDisconnectTimeout()
{
    wsParser->disconnectFromStream();
    //showMessage("Auto Disconnect", "Stream automatically disconnected after timeout", QMessageBox::Information);
}

void Scanner::onTriggeredRuleTimer() {
    static int nScan = 2;

    if (!triggeredRuleTimer->isActive() && pScannerConfig.durations.size() >= nScan) {
        triggeredRuleTimer->start(pScannerConfig.durations[nScan-1]);
        nScan++;
    }

    auto exp = myWalletParser->parseAllRules();
    if (exp.has_value())
        statPanel->displayResults(exp.value());
    /*else
        outputResult->append(QString("Error : %1").arg(exp.error()));*/
}

void Scanner::start() {

    statPanel->show();

    wsParser->subscribeToCoins(pScannerConfig.pairs);
    wsParser->connectToStream(WebSocketParser::TradeUrl);

    if (pScannerConfig.enableAutoDisconnect)
        autoDisconnectTimer->start(pScannerConfig.allDuration + 1000);

    if (pScannerConfig.durations.size() > 0)
        triggeredRuleTimer->start(pScannerConfig.durations[0]);

}

void Scanner::stop() {

    if (autoDisconnectTimer->isActive())
       autoDisconnectTimer->stop();

    wsParser->disconnectFromStream();
    statPanel->close();

}

void Scanner::onPriceUpdated(const QString &coin, double price)
{
    // Вызывается мгновенно при изменении цены на Binance
    myWalletParser->updateAllAssets(coin, price);
    myWalletParser->updateWallet();
    statPanel->display();
}

void Scanner::onWebSocketConnected() {
    //outputResult->append("Connected to Binance WebSocket");
}

void Scanner::onWebSocketDisconnected() {
    //outputResult->append("Disconnected from Binance WebSocket");
}

void Scanner::onWebSocketError(const QString &error) {
   //outputResult->append("WebSocket error: " + error);
}