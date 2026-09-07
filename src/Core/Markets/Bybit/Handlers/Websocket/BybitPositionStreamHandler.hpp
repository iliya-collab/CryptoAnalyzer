#pragma once
#include "Markets/Websocket/IStreamHandler.hpp"

namespace Core::Markets
{

    class BybitPositionStreamHandler : public IPrivateStreamHandler
    {
    public:

        static QString topic() { return "position"; }

        void handle(const QJsonObject& data, IPrivateMarketDataStreamer* streamer) override;
    };

}
