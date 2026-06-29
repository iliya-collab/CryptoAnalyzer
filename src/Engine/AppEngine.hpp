#pragma once
#include <QObject>
#include <memory>
#include "Tools/Network/BybitWebSocket.hpp"
 
namespace Engine {

    class AppEngine : public QObject {
        Q_OBJECT
    private:

        std::unique_ptr<BybitWebSocket> m_webSocket;

        qint64 m_startTime;
        bool m_isInterrupt = false;
        QString m_lastPair = "";

        QMap<QString, Orderbook> m_orderBooks; // Список ордеров
        QList<Kline> m_savedCandles;

        void updateOrderbook(Orderbook& oldOrderbook, const Orderbook& newOrderbook);
        void snapshotOrderbook(Orderbook& oldOrderbook, const Orderbook& newOrderbook);
        void deltaUpdateOrderbook(Orderbook& oldOrderbook, const Orderbook& newOrderbook);

    public:

        AppEngine(QObject* parent = nullptr);
        ~AppEngine();

        void start();
        void stop(bool interrupt = false);
        void setAPI(const API& api);
        void addTrade(const QString& pair);
        bool hasRunned();
        qint64 getStartTime();

    signals:

        void started();
        void stopped();
        void errorOccurred(const QString& error);

        void tickerUpdated(const Ticker& newTicker);
        void orderBookUpdated(const Orderbook& newOrderBook);
        void klineUpdated(const Kline& newKline);

    };

}