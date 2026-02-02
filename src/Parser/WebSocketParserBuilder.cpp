#include "Parser/WebSocketParserBuilder.hpp"

class BybitParserBuilder : public WebSocketParserBuilder {
public:
    BybitParserBuilder(const QString& Market) {
        m_webSocketParser = std::make_unique<BybitParser>(QString("Bybit/%1").arg(Market));
    }
};

WebSocketParser* WebSocketParserBuilder::parser() {
    return m_webSocketParser.get();
};

WebSocketParserBuilder* WebSocketParserBuilder::createParser(const QString& StockMarket, const QString& Market) {
    if (StockMarket == "Bybit")
        return new BybitParserBuilder(Market);
    return nullptr;
}