#pragma once
#include "Markets/Websocket/IStreamHandler.hpp"

namespace Core::Markets
{

    class BybitOrderbookStreamHandler : public IPublicStreamHandler
    {
    private:

        Tools::Orderbook m_savedOrderbook{};

        void snapshotOrderbook(Tools::Orderbook& oldOrderbook, const Tools::Orderbook& newOrderbook);
        void deltaUpdateOrderbook(Tools::Orderbook& oldOrderbook, const Tools::Orderbook& newOrderbook);

    public:

        void handle(const QJsonObject& obj, IPublicMarketDataStreamer* streamer) override;

    };

}

