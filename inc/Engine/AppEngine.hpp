#pragma once

#include "Engine/WebSocket.hpp"
#include "Engine/WebSocketBuilder.hpp"

#include <memory>

class AppEngine : public QObject {
    Q_OBJECT
private:

    std::unique_ptr<Engine::WebSocket> m_webSocket = nullptr;

    QString m_stockMarket;
    QString m_market;
    QString m_coin;

public: 

    void initEngine(const QString& StockMarket, const QString& Market, const QString& Coin);

    void runWebSocket();
    void stopWebSocket();
    bool isRunning();

    void changeCoin(const QString& Coin);

};