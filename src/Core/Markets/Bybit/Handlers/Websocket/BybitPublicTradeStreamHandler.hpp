#pragma once
#include "Markets/Websocket/IStreamHandler.hpp"

namespace Core::Markets
{

    class BybitPublicTradeStreamHandler : public IMarketDataStreamHandler
    {
    public:

        static QString topic() { return "publicTrade"; }

        void handle(const QJsonObject& obj, IMarketDataStreamer* streamer) override;
    };

}

