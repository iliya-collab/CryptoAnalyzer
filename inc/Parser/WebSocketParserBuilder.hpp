#pragma once

#include "Parser/BinanceParser.hpp"
#include "Parser/BybitParser.hpp"
#include "Parser/OKXParser.hpp"
#include "Parser/WebSocketParser.hpp"

#include <memory>


class WebSocketParserBuilder {
protected:

    std::unique_ptr<WebSocketParser> _webSocketParser;

public:

    virtual ~WebSocketParserBuilder() = default;

    static WebSocketParserBuilder* createParser(const QString& StockMarket, const QString& Market);
    WebSocketParser* parser();
};