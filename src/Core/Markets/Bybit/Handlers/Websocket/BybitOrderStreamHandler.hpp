#pragma once
#include "Markets/Websocket/IStreamHandler.hpp"

namespace Core::Markets
{

    class BybitOrderStreamHandler : public IPrivateStreamHandler
    {
    public:
        void handle(const QJsonObject& data, IPrivateMarketDataStreamer* streamer) override;
    };

}


