#pragma once
#include "Markets/IStreamHandler.hpp"

namespace Core::Markets
{

    class BybitOrderbookStreamHandler : public IStreamHandler
    {
    private:

        Tools::Orderbook m_savedOrderbook{};

        void snapshotOrderbook(Tools::Orderbook& oldOrderbook, const Tools::Orderbook& newOrderbook);
        void deltaUpdateOrderbook(Tools::Orderbook& oldOrderbook, const Tools::Orderbook& newOrderbook);

    public:

        void handle(const QJsonObject& obj, IMarketDataStreamer* streamer) override;

    };

}

