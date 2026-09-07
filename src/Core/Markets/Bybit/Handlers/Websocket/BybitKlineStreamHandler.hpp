#pragma once
#include "Markets/Websocket/IStreamHandler.hpp"

namespace Core::Markets
{

    class BybitKlineStreamHandler : public IPublicStreamHandler
    {
    public:

        static QString topic() { return "kline"; }

        void handle(const QJsonObject& obj, IPublicMarketDataStreamer* streamer) override;
    };

}


