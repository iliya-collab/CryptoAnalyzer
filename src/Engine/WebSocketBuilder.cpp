#include "Engine/WebSocketBuilder.hpp"

class BybitMarketBuilder : public Engine::WebSocketBuilder {
public:
    BybitMarketBuilder(const QString& Market) {
        if (Market == "spot")
            m_webSocket = std::make_unique<Engine::BybitWebSocketSpot>();
        else if (Market == "futures")
            m_webSocket = std::make_unique<Engine::BybitWebSocketFutures>();
    }
};

Engine::WebSocket* Engine::WebSocketBuilder::get() {
    return m_webSocket.get();
};

Engine::WebSocketBuilder* Engine::WebSocketBuilder::create(const QString& StockMarket, const QString& Market) {
    if (StockMarket == "Bybit")
        return new BybitMarketBuilder(Market);
    return nullptr;
}