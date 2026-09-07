#pragma once
#include "BybitWebSocket.hpp"

namespace Core::Tools
{

    class BybitWebSocketCreater
    {
    public:

        static std::unique_ptr<BybitWebSocket> create(SocketType socketType, MarketType marketType, const Api& api, QObject* parent = nullptr)
        {
            auto newBybitWebSocket = std::make_unique<BybitWebSocket>(socketType, marketType, parent);
            newBybitWebSocket->setId(marketTypeToString(marketType));
            newBybitWebSocket->init(api);
            return newBybitWebSocket;
        }

    };

}