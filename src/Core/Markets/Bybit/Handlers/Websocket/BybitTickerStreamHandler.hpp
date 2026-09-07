#pragma once
#include "Markets/Websocket/IStreamHandler.hpp"

namespace Core::Markets
{

    class BybitTickerStreamHandler : public IPublicStreamHandler
    {
    public:

        void handle(const QJsonObject& obj, IPublicMarketDataStreamer* streamer) override;

    };

}

