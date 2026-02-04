#include "Engine/AppEngine.hpp"

void AppEngine::initEngine(const QString& StockMarket, const QString& Market, const QString& Coin) {
    m_stockMarket = StockMarket;
    m_market = Market;
    m_coin = Coin;

    m_webSocket = std::unique_ptr<Engine::WebSocket>(
        Engine::WebSocketBuilder::create(m_stockMarket, m_market)->get()
    );

    m_webSocket->subscribeToCoin(m_coin);
}

void AppEngine::runWebSocket() {
    if (!m_webSocket)
        return;

    m_webSocket->connectToStream();
}

void AppEngine::stopWebSocket() {
    if (!m_webSocket)
        return;

    m_webSocket->disconnectFromStream();
}

bool AppEngine::isRunning() {
    if (m_webSocket && m_webSocket->isConnected())
        return true;
    return false;
}

void AppEngine::changeCoin(const QString& Coin) {
    if (m_webSocket) {
        m_coin = Coin;
        m_webSocket->unsubscribeFromCoin(m_coin);
        m_webSocket->subscribeToCoin(Coin);
    }
}