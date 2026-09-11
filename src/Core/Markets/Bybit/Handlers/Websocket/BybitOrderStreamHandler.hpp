#pragma once
#include "Markets/Websocket/IStreamHandler.hpp"

namespace Core::Markets
{

    class BybitOrderStreamHandler : public IPrivateStreamHandler
    {
    public:

        static QString topic() { return "order"; }

        void handle(const QJsonObject& data, IPrivateStreamer* streamer) override;
    };

}


