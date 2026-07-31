#pragma once
#include "Tools/Network/BybitWebSocket.hpp"
#include <QObject>
#include <memory>
 
namespace Core {

    class MarketDataStreamer : public QObject {
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

        MarketDataStreamer(QObject* parent = nullptr);
        ~MarketDataStreamer();

        void start();
        void stop(bool interrupt = false);
        void setAPI(const Tools::API& api);
        void addTrade(const QString& pair);
        bool hasRunned();

    signals:

        void started();
        void stopped();
        void errorOccurred(const QString& error);
        void pingMeasured(qint64 pingMs);

        void tickerUpdated(const Tools::Ticker& newTicker);
        void orderBookUpdated(const Tools::Orderbook& newOrderBook);
        void klineUpdated(const Tools::Kline& newKline);
        void publicTradeUpdated(const Tools::PublicTrades& newPublicTrades);

    };

}