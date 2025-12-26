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
    statPanel->displayMessages("Auto Disconnect : Stream automatically disconnected after timeout");
}

void Scanner::onTriggeredRuleTimer() {
    static int nScan = 1;

    auto exp = myWalletParser->parseAllRules();
    if (exp.has_value()) {
        statPanel->clearDisplay(StatusPanel::Display::StatWallet);
        statPanel->outputStringInDisplay(StatusPanel::Display::StatWallet, 
            QString("Scan #%1\nRevenue : %2")
            .arg(nScan)
            .arg(QString::number(exp.value(), 'f', 4)));
        statPanel->displayStatWallet();
    }
    else
        statPanel->displayMessages(QString("Error : %1").arg(exp.error()));

    nScan++;

    if (!triggeredRuleTimer->isActive() && pScannerConfig.durations.size() >= nScan)
        triggeredRuleTimer->start(pScannerConfig.durations[nScan-1]);
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
    statPanel->clearDisplay(StatusPanel::Display::StatAssets);
    statPanel->displayStatAssets();
}

void Scanner::onWebSocketConnected() {
    statPanel->displayMessages("Connected to WebSocket");
}

void Scanner::onWebSocketDisconnected() {
    statPanel->displayMessages("Disconnected from WebSocket");
}

void Scanner::onWebSocketError(const QString &error) {
   statPanel->displayMessages("WebSocket error: " + error);
}