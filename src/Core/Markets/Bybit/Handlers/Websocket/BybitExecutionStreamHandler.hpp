#pragma once
#include "Markets/Websocket/IStreamHandler.hpp"

namespace Core::Markets
{

    class BybitExecutionStreamHandler : public IPrivateStreamHandler
    {
    public:
        void handle(const QJsonObject& data, IPrivateMarketDataStreamer* streamer) override;
    };

}