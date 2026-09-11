#pragma once
#include "IMarketDataStreamer.hpp"
#include "IPrivateStreamer.hpp"
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

    using IMarketDataStreamHandler = IStreamHandler<IMarketDataStreamer>;
    using IPrivateStreamHandler = IStreamHandler<IPrivateStreamer>;
}
