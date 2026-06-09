#pragma once

#include <QObject>
#include <memory>

#include "Engine/Tools/BybitWebSocket.hpp"
 
namespace Engine {

    class AppEngine : public QObject {
        Q_OBJECT
    private:

        std::unique_ptr<BybitWebSocket> m_webSocket;

        bool m_isInterrupt = false;
        QString m_lastPair = "";

        QMap<QString, stOrderBook> m_orderBooks; // Список ордеров
        QList<stKline> m_savedCandles;

        void updateOrderbook(stOrderBook& oldOrderbook, const stOrderBook& newOrderbook);
        void snapshotOrderbook(stOrderBook& oldOrderbook, const stOrderBook& newOrderbook);
        void deltaUpdateOrderbook(stOrderBook& oldOrderbook, const stOrderBook& newOrderbook);

    public:

        AppEngine(QObject* parent = nullptr);
        ~AppEngine();

        void start();
        void stop(bool interrupt = false);
        void setAPI(const API& api);
        void addTrade(const QString& pair);
        bool hasRunned();

    signals:

        void started();
        void stopped();
        void errorOccurred(const QString& error);

        void tickerUpdated(const stTicker& newTicker);
        void orderBookUpdated(const stOrderBook& newOrderBook);
        void klineUpdated(const stKline& newKline);

    };

}