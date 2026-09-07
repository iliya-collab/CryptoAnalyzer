#pragma once
#include "Markets/Websocket/IStreamHandler.hpp"

namespace Core::Markets
{

    class BybitPublicTradeStreamHandler : public IPublicStreamHandler
    {
    public:

        static QString topic() { return "publicTrade"; }

        void handle(const QJsonObject& obj, IPublicMarketDataStreamer* streamer) override;
    };

}

