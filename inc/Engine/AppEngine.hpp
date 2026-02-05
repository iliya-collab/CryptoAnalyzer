#pragma once

#include "Configs/PlatformConfig.hpp"
#include "Engine/WebSocket.hpp"
#include "Engine/WebSocketBuilder.hpp"
#include "Engine/BybitRestAPI.hpp"

#include <memory>

class AppEngine : public QObject {
    Q_OBJECT
public:

    struct InfoAboutAPI {
        QString m_apiKey;
        QString m_secretKey;
        bool m_testnet;
    };
    
    struct EngineLaunchParams {
        QString m_stockMarket;
        QString m_market;
        QString m_coin;
    };

private:

    std::unique_ptr<Engine::WebSocket> m_webSocket = nullptr;

    InfoAboutAPI m_api;
    EngineLaunchParams m_params;

public: 

    void setAPI(const InfoAboutAPI& api);

    void setLaunchParams(const EngineLaunchParams& p_launch);

    void getInfoAboutMarket();

    void run();
    void stop();
    bool isRunning();

};