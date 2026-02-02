#pragma once

#include "Parser/BybitParser.hpp"
#include "Parser/WebSocketParser.hpp"

#include <memory>


class WebSocketParserBuilder {
protected:

    WebSocketParserBuilder() = default;
    
    std::unique_ptr<WebSocketParser> m_webSocketParser;

public:

    virtual ~WebSocketParserBuilder() = default;

    static WebSocketParserBuilder* createParser(const QString& StockMarket, const QString& Market);
    WebSocketParser* parser();
};