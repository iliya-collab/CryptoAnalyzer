#pragma once
#include "Tools/Network/SocketType.hpp"
#include "Tools/StdTypes.hpp"
#include <QString>

namespace Core::Tools
{

    class BybitEndpointProvider
    {
    public:

        // Для WebSocket
        static QString webSocketUrl(SocketType socketType, MarketType marketType, bool isTestnet)
        {
            const QString base = isTestnet ? "wss://stream-testnet.bybit.com" : "wss://stream.bybit.com";

            if (socketType == SocketType::Private)
                return base + "/v5/private";

            return base + "/v5/public/" + marketTypeToString(marketType);
        }

        // Для REST API
        static QString restBaseUrl(bool isTestnet)
        {
            return isTestnet ? "https://api-testnet.bybit.com" : "https://api.bybit.com";
        }
    };

}



