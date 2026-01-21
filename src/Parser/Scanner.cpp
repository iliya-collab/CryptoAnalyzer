#include "Parser/Scanner.hpp"
#include "Parser/WebSocketParserBuilder.hpp"

#include <memory>

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
    const QString name = QString("%1/%2").arg(StockMarket).arg(Market);
    
    // Проверка существования парсера
    if (lstParsers.contains(name)) {
        lstParsers[name]->addChannels(channel);
        return;
    }
    
    // Создание нового парсера
    auto parser = std::make_shared<WebSocketParser>(WebSocketParserBuilder::createParser(StockMarket, Market)->parser());
    if (!parser || !parser->init()) {
        qDebug() << "Failed to create parser for" << name;
        return;
    }
    
    lstParsers[name] = parser;
    parser->addChannels(channel);
    setupParserConnections(parser.get(), channel);
    
    qDebug() << "Created" << name;

}

void Scanner::setupParserConnections(WebSocketParser* parser, const QString& channel) {
    if (channel == "ticker")
        connect(parser, &WebSocketParser::updatedTicker, this, &Scanner::updateTicker, Qt::UniqueConnection); 
    else if (channel.startsWith("books"))
        connect(parser, &WebSocketParser::updatedOrderBooks, this, &Scanner::updateOrderBooks, Qt::UniqueConnection);
}

void Scanner::delStockMarket(const QString& StockMarket, const QString& Market) {
    QString name = QString("%1/%2").arg(StockMarket).arg(Market);
    if (!lstParsers.contains(name)) {
        qWarning() << name << "does not exist";
        return;
    }
    lstParsers.remove(name);
    qDebug() << name << "was deleted";
}

QStringList Scanner::getListStockMarket() {
    return lstParsers.keys();
}

void Scanner::setScannerConfig(const ParamsScannerConfig& _config) {
    pScannerConfig = _config;
}

ParamsScannerConfig Scanner::getScannerConfig() {
    return pScannerConfig;
}

void Scanner::updateTicker(const WebSocketParser::stTicker& _ticker){
    qDebug() << _ticker.namePair << "ticker";
    emit ticker(_ticker);
}

void Scanner::updateOrderBooks(const WebSocketParser::stOrderBooks& _orderBooks) {
    qDebug() << _orderBooks.namePair << "order books";
    emit orderBooks(_orderBooks);
}
