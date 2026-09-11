#pragma once
#include "Markets/Websocket/IStreamHandler.hpp"

namespace Core::Markets
{

    class BybitKlineStreamHandler : public IMarketDataStreamHandler
    {
    public:

        static QString topic() { return "kline"; }

        void handle(const QJsonObject& obj, IMarketDataStreamer* streamer) override;
    };

}


