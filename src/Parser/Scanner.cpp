#include "Parser/Scanner.hpp"

#include "Parser/WebSocketParserBuilder.hpp"

#include <memory>

Scanner::Scanner(QWidget *parent) {
    myWalletParser = std::make_unique<ParserMyWallet>();
}

void Scanner::setConfig(const ParamsScannerConfig& _ScannerConfig, const ParamsMyWalletConfig& _MyWalletConfig) {
    pScannerConfig = _ScannerConfig;
    pMyWalletConfig = _MyWalletConfig;
}

void Scanner::start() {
    for (auto [name, parser] : lstParsers.asKeyValueRange()) {
        parser->subscribeToCoins(pScannerConfig.pairs);
        parser->connectToStream();
    }
}

void Scanner::stop() {
    for (auto [name, parser] : lstParsers.asKeyValueRange())
        parser->disconnectFromStream();
}

void Scanner::addStockMarket(const QString& StockMarket, const QString& Market, const QString& channel) {
    qDebug() << StockMarket << Market << channel;
    QString name = QString("%1/%2").arg(StockMarket).arg(Market);
    bool isInit = true;

    if (!lstParsers.contains(name)) {
        auto new_parser = std::shared_ptr<WebSocketParser>(WebSocketParserBuilder::createParser(StockMarket, Market)->parser());
        if ((isInit = new_parser->init())) {
            lstParsers[name] = new_parser;
            connect(lstParsers[name].get(), &WebSocketParser::updated, this, &Scanner::updateCoin);
            qDebug() << QString("Created %1 :").arg(name) << lstParsers[name].get();
        }
    }

    if (isInit)
        lstParsers[name]->addChannels(channel);
}

QStringList Scanner::getListStockMarket() {
    return lstParsers.keys();
}

void Scanner::updateCoin(const QString &symbol, const WebSocketParser::stInfoCoin& _info) {
    qDebug() << symbol << QString::number(_info.stPrice.curPrice, 'f', 3);
    emit update_coin(symbol, _info);
}
