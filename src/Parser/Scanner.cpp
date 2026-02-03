#include "Parser/Scanner.hpp"
/*#include "Parser/RegisterParsers.hpp"
#include "Parser/WebSocketParserBuilder.hpp"

#include <memory>

void Scanner::start() {
    for (auto [name, parser] : lstParsers.asKeyValueRange()) {
        parser->subscribeToCoins(m_platform_config.pairs);
        parser->connectToStream();
    }
}

void Scanner::stop() {
    for (auto [name, parser] : lstParsers.asKeyValueRange())
        parser->disconnectFromStream();
}

void Scanner::addStockMarket(const QString& StockMarket) {
    if (lstParsers.contains(StockMarket))
        return;

    RegisterParsers::instanse().registerParser(StockMarket);
    
    QStringList lst = StockMarket.split('/');
    auto parser = std::shared_ptr<WebSocketParser>(WebSocketParserBuilder::createParser(lst[0], lst[1])->parser());
    if (!parser || !parser->init()) {
        qDebug() << "Failed to create parser for" << StockMarket;
        return;
    }
    
    lstParsers[StockMarket] = parser;
    qDebug() << "Created" << StockMarket;
}

void Scanner::setupParserConnections(WebSocketParser* parser, const QSet<QString>& channels) {
    for (auto channel : channels)
        if (channel == "ticker")
            connect(parser, &WebSocketParser::updatedTicker, this, &Scanner::updateTicker, Qt::UniqueConnection); 
        else if (channel == "books")
            connect(parser, &WebSocketParser::updatedOrderBooks, this, &Scanner::updateOrderBooks, Qt::UniqueConnection);
}

void Scanner::delStockMarket(const QString& StockMarket) {
    RegisterParsers::instanse().deleteParser(StockMarket);

    if (!lstParsers.contains(StockMarket)) {
        qWarning() << StockMarket << "does not exist";
        return;
    }
    lstParsers.remove(StockMarket);
    qDebug() << StockMarket << "was deleted";
}

void Scanner::addChannels(const QString& StockMarket, const QSet<QString>& channels) {
    lstParsers[StockMarket]->deleteAllChannels();

    auto lst = channels.values();
    for (auto channel : lst)
        lstParsers[StockMarket]->addChannels(channel);

    setupParserConnections(lstParsers[StockMarket].get(), channels);
}

QStringList Scanner::getListStockMarket() {
    return lstParsers.keys();
}

void Scanner::setConfig(const ParamsPlatformConfig& _config) {
    m_platform_config = _config;
}

ParamsPlatformConfig Scanner::getConfig() {
    return m_platform_config;
}

void Scanner::updateTicker(const WebSocketParser::stTicker& _ticker){
    emit ticker(_ticker);
}

void Scanner::updateOrderBooks(const WebSocketParser::stOrderBooks& _orderBooks) {
    emit orderBooks(_orderBooks);
}*/
