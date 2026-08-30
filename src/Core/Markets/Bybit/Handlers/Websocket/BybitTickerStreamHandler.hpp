#pragma once
#include "Markets/IStreamHandler.hpp"

namespace Core::Markets
{

    class BybitTickerStreamHandler : public IStreamHandler
    {
    public:

        void handle(const QJsonObject& obj, IMarketDataStreamer* streamer) override;

    };

}

