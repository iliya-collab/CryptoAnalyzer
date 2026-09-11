#pragma once
#include "Markets/Websocket/IStreamHandler.hpp"

namespace Core::Markets
{

    class BybitOrderbookStreamHandler : public IMarketDataStreamHandler
    {
    private:

        Tools::Orderbook m_savedOrderbook{};

        void snapshotOrderbook(Tools::Orderbook& oldOrderbook, const Tools::Orderbook& newOrderbook);
        void deltaUpdateOrderbook(Tools::Orderbook& oldOrderbook, const Tools::Orderbook& newOrderbook);

    public:

        static QString topic() { return "orderbook"; }

        void handle(const QJsonObject& obj, IMarketDataStreamer* streamer) override;

    };

}

