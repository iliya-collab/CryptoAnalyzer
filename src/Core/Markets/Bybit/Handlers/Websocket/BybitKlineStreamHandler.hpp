#pragma once
#include "Markets/IStreamHandler.hpp"

namespace Core::Markets
{

    class BybitKlineStreamHandler : public IStreamHandler
    {
    public:
        void handle(const QJsonObject& obj, IMarketDataStreamer* streamer) override;
    };

}


