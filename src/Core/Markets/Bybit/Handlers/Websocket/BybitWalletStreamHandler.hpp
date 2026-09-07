#pragma once
#include "Markets/Websocket/IStreamHandler.hpp"

namespace Core::Markets
{

    class BybitWalletStreamHandler : public IPrivateStreamHandler
    {
    public:

        void handle(const QJsonObject& data, IPrivateMarketDataStreamer* streamer) override;

    };

}