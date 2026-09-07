#pragma once
#include "IPublicMarketDataStreamer.hpp"
#include "IPrivateMarketDataStreamer.hpp"
#include <QJsonObject>

namespace Core::Markets
{

    template <typename TStreamer>
    class IStreamHandler
    {
    public:
        virtual ~IStreamHandler() = default;
        virtual void handle(const QJsonObject& data, TStreamer* streamer) = 0;
    };

    using IPublicStreamHandler = IStreamHandler<IPublicMarketDataStreamer>;
    using IPrivateStreamHandler = IStreamHandler<IPrivateMarketDataStreamer>;
}
