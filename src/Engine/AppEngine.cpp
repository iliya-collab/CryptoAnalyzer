#include "Engine/AppEngine.hpp"

void AppEngine::setLaunchParams(const EngineLaunchParams& p_launch) {
    m_params = p_launch;
}

void AppEngine::setAPI(const InfoAboutAPI& api) {
    m_api = api;
}

void AppEngine::run() {
    if (!m_webSocket)
        return;

    m_webSocket = std::unique_ptr<Engine::WebSocket>(
        Engine::WebSocketBuilder::create(m_params.m_stockMarket , m_params.m_market)->get()
    );

    m_webSocket->subscribeToCoin(m_params.m_coin);
    m_webSocket->connectToStream();
}

void AppEngine::stop() {
    if (!m_webSocket)
        return;

    m_webSocket->disconnectFromStream();
}

bool AppEngine::isRunning() {
    if (m_webSocket && m_webSocket->isConnected())
        return true;
    return false;
}

void AppEngine::getInfoAboutMarket() {

}