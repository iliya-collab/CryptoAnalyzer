#include "Parser/WebSocketParserBuilder.hpp"

class BinanceParserBuilder : public WebSocketParserBuilder {
public:
    BinanceParserBuilder(const QString& Market) {
        _webSocketParser = std::make_unique<BinanceParser>(QString("Binance/%1").arg(Market));
    }
};

class BybitParserBuilder : public WebSocketParserBuilder {
public:
    BybitParserBuilder(const QString& Market) {
        _webSocketParser = std::make_unique<BybitParser>(QString("Bybit/%1").arg(Market));
    }
};

class OKXParserBuilder : public WebSocketParserBuilder {
public:
    OKXParserBuilder(const QString& Market) {
        _webSocketParser = std::make_unique<OKXParser>(QString("OKX/%1").arg(Market));
    }
};

WebSocketParser* WebSocketParserBuilder::parser() {
    return _webSocketParser.get();
};

WebSocketParserBuilder* WebSocketParserBuilder::createParser(const QString& StockMarket, const QString& Market) {
    if (StockMarket == "Binance")
        return new BinanceParserBuilder(Market);
    else if (StockMarket == "Bybit")
        return new BybitParserBuilder(Market);
    else if (StockMarket == "OKX")
        return new OKXParserBuilder(Market);
    return nullptr;
}