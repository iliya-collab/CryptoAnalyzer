#include "Parser/Scanner.hpp"

#include "Parser/BinanceParser.hpp"
#include "Parser/BybitParser.hpp"
#include "Parser/OKXParser.hpp"
#include "Parser/WebSocketParser.hpp"
#include <memory>

Scanner::Scanner(QWidget *parent) {

    statPanel = std::make_unique<StatusPanel>(parent);
    myWalletParser = std::make_unique<ParserMyWallet>();

    initParsers();
    connectSignals();

}

void Scanner::connectSignals() {
    connect(statPanel.get(), &StatusPanel::clicked_btn, this, &Scanner::onStartConnection);
    connect(statPanel.get(), &StatusPanel::triggered_table, this, &Scanner::onDialogTable);
    /*for (auto [name, parser] : lstParsers.asKeyValueRange())
        connect(parser.get(), &WebSocketParser::updated, this, &Scanner::updateTable);*/
}

void Scanner::initParsers() {
    std::shared_ptr<BinanceParser> binance_spot = std::make_shared<BinanceParser>("Binance/spot", WebSocketParser::TMarketData::SPOT, this);
    std::shared_ptr<BinanceParser> binance_futures = std::make_shared<BinanceParser>("Binance/futures", WebSocketParser::TMarketData::FUTURES, this);
    /*std::shared_ptr<BybitParser> bybit_spot = std::make_shared<BybitParser>("Bybit/spot", WebSocketParser::TMarketData::SPOT, this);
    std::shared_ptr<BybitParser> bybit_futures = std::make_shared<BybitParser>("Bybit/futures", WebSocketParser::TMarketData::FUTURES, this);
    std::shared_ptr<OKXParser> okx_spot = std::make_shared<OKXParser>("OKX/spot", WebSocketParser::TMarketData::SPOT, this);
    std::shared_ptr<OKXParser> okx_futures = std::make_shared<OKXParser>("OKX/futures", WebSocketParser::TMarketData::FUTURES, this);*/

    lstParsers[binance_spot->getNameMarket()] = binance_spot;
    lstParsers[binance_futures->getNameMarket()] = binance_futures;
    /*lstParsers[bybit_spot->getNameMarket()] = bybit_spot;
    lstParsers[bybit_futures->getNameMarket()] = bybit_futures;
    lstParsers[okx_spot->getNameMarket()] = okx_spot;
    lstParsers[okx_futures->getNameMarket()] = okx_futures;*/
}

void Scanner::setConfig(const ParamsScannerConfig& _ScannerConfig, const ParamsMyWalletConfig& _MyWalletConfig) {
    pScannerConfig = _ScannerConfig;
    pMyWalletConfig = _MyWalletConfig;
}

void Scanner::start() {
    statPanel->show();
}

void Scanner::stop() {
    for (auto [name, parser] : lstParsers.asKeyValueRange())
        parser->disconnectFromStream();
}

void Scanner::updateTable(const QString &coin, double price) {
    qDebug() << coin << QString::number(price, 'f', 3);
    statPanel->getTable()->updateTable(coin, price);
}

void Scanner::onDialogTable() {
    QStringList lst = pScannerConfig.pairs;
    for (auto& pair : lst)
        pair.replace("/", "");
    statPanel->getTable()->showTable(lstParsers.size(), pScannerConfig.pairs.size(), lst, lstParsers.keys());
}

void Scanner::onDialogGraph() {

}

void Scanner::onStartConnection() {
    for (auto [name, parser] : lstParsers.asKeyValueRange()) {
        parser->subscribeToCoins(pScannerConfig.pairs);
        parser->connectToStream();
    }
}
