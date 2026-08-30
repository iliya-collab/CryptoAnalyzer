#pragma once
#include "IMarketDataStreamer.hpp"
#include <QJsonObject>

namespace Core::Markets
{

    class IStreamHandler
    {
    public:

        virtual ~IStreamHandler() = default;

        virtual void handle(const QJsonObject& obj, IMarketDataStreamer* streamer) = 0;

    };

}
