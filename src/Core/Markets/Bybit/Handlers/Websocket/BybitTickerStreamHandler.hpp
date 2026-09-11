#pragma once
#include "Markets/Websocket/IStreamHandler.hpp"

namespace Core::Markets
{

    class BybitTickerStreamHandler : public IMarketDataStreamHandler
    {
    public:

        static QString topic() { return "tickers"; }

        void handle(const QJsonObject& obj, IMarketDataStreamer* streamer) override;

    };

}

