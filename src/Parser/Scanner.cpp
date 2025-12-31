#include "Parser/Scanner.hpp"
#include "Configs/MyWalletConfig.hpp"
#include "CustomWindowDialogs/StatusPanel.hpp"
#include "Parser/BinanceParser.hpp"

Scanner::Scanner(QWidget *parent) {

    isAutoDisconnected = false;
    isDisconnected = false;
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

void Scanner::onDisconnectTimeout() {
    isAutoDisconnected = true;
    stop();
}

void Scanner::onTriggeredRuleTimer() {

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

void Scanner::getRequestedCoin() {
    requestedCoin.clear();
    auto cur_asset = MyWalletConfig::instance().getConfig().asset;
    for (auto [coin, amount] : cur_asset.asKeyValueRange())
        requestedCoin.push_back(coin);
}

void Scanner::start() {

    nScan = 1;
    isDisconnected = false;

    statPanel->show();

    wsParser->subscribeToCoins(pScannerConfig.pairs);
    wsParser->connectToStream(WebSocketParser::TradeUrl);
}

void Scanner::startSnapshotCheck() {
    statPanel->displayMessages("Waiting to receive a snapshot...");

    getRequestedCoin();

    snapshotTimer = std::make_unique<QTimer>();
    snapshotTimer->setInterval(100);
    connect(snapshotTimer.get(), &QTimer::timeout, this, &Scanner::checkSnapshot);
    snapshotTimer->start();
}

void Scanner::checkSnapshot() {
    auto allCoins = BinanceParser::getInfoAboutAllCoins();

    for (auto next_coin = requestedCoin.begin(); next_coin != requestedCoin.end(); ) {
        if (allCoins.contains(*next_coin)) {
            statPanel->displayMessages(QString("%1 - received").arg(*next_coin));
            next_coin = requestedCoin.erase(next_coin);
        }
        else
            ++next_coin;
    }

    if (requestedCoin.empty() && snapshotTimer->isActive()) {
        snapshotTimer->stop();
        statPanel->displayMessages("All snapshots received");

        if (pScannerConfig.enableAutoDisconnect && !autoDisconnectTimer->isActive())
            autoDisconnectTimer->start(pScannerConfig.allDuration + 1000);

        if (pScannerConfig.durations.size() > 0 && !triggeredRuleTimer->isActive())
            triggeredRuleTimer->start(pScannerConfig.durations[nScan-1]);
    }

}

void Scanner::stop() {
    wsParser->disconnectFromStream();
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
    startSnapshotCheck();
}

void Scanner::onWebSocketDisconnected() {
    if (!isDisconnected) {
        if (isAutoDisconnected) 
            statPanel->displayMessages("Auto disconnected from WebSocket");
        else
            statPanel->displayMessages("Disconnected from WebSocket");

        isDisconnected = true;
        isAutoDisconnected = false;
        autoDisconnectTimer->stop();
    }
}

void Scanner::onWebSocketError(const QString &error) {
   statPanel->displayMessages("WebSocket error: " + error);
}
