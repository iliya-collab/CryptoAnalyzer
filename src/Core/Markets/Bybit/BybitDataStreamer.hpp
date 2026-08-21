#pragma once
#include "Markets/IMarketDataStreamer.hpp"
#include "Tools/Network/BybitWebSocket.hpp"
 
namespace Core::Markets {

/*
 *  Класс для работы с потоковыми данными (ByBit Websocket API)
*/
    class BybitDataStreamer : public IMarketDataStreamer {
        Q_OBJECT
    private:

        std::unique_ptr<Tools::BybitWebSocket> m_webSocket;

        bool m_isInterrupt = false;
        QString m_lastPair = "";

        Tools::Orderbook m_orderBook; // стакан заявок
        QList<Tools::Kline> m_savedCandles; // серия свеч

        void updateOrderbook(Tools::Orderbook& oldOrderbook, const Tools::Orderbook& newOrderbook);
        void snapshotOrderbook(Tools::Orderbook& oldOrderbook, const Tools::Orderbook& newOrderbook);
        void deltaUpdateOrderbook(Tools::Orderbook& oldOrderbook, const Tools::Orderbook& newOrderbook);

    public:

        BybitDataStreamer(QObject* parent = nullptr);
        ~BybitDataStreamer();

        void setAPI(const Tools::API& api) override;
        void start() override;
        void stop() override;
        void restart() override;
        void subscribeSymbol(const QString& symbol) override;
        bool hasRunned() override;

    };

}