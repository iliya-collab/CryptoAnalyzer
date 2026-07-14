#pragma once
#include <QObject>
#include <memory>
#include "Tools/Network/BybitWebSocket.hpp"
 
namespace Core {

    class MarketDataStreamer : public QObject {
        Q_OBJECT
    private:

        std::unique_ptr<Tools::BybitWebSocket> m_webSocket;

        bool m_isInterrupt = false;
        QString m_lastPair = "";

        QMap<QString, Tools::Orderbook> m_orderBooks; // Список ордеров
        QList<Tools::Kline> m_savedCandles;

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

        void tickerUpdated(const Tools::Ticker& newTicker);
        void orderBookUpdated(const Tools::Orderbook& newOrderBook);
        void klineUpdated(const Tools::Kline& newKline);

    };

}