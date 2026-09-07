#pragma once
#include "Markets/Websocket/IStreamHandler.hpp"

namespace Core::Markets
{

    class BybitWalletStreamHandler : public IPrivateStreamHandler
    {
    public:

        static QString topic() { return "wallet"; }

        void handle(const QJsonObject& data, IPrivateMarketDataStreamer* streamer) override;

    };

}