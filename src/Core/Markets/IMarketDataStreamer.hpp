#pragma once
#include <Tools/StdTypes.hpp>
#include <QObject>

namespace Core::Markets {

    /*
     *  Класс для работы с потоковыми данными
    */
    class IMarketDataStreamer : public QObject {
        Q_OBJECT
    public:

        IMarketDataStreamer(QObject* parent = nullptr) : QObject(parent) {};
        virtual ~IMarketDataStreamer() {}

        virtual void setAPI(const Tools::API& api) = 0;
        virtual void start() = 0;
        virtual void stop() = 0;
        virtual void restart() = 0;
        virtual void subscribeSymbol(const QString& symbol) = 0;
        virtual bool hasRunned() = 0;

    signals:
        void started();
        void stopped();
        void pingMeasured(qint64 pingMs);
        void errorOccurred(const QString& error);
        void tickerUpdated(const Tools::Ticker& newTicker);
        void orderBookUpdated(const Tools::Orderbook& newOrderBook);
        void klineUpdated(const Tools::Kline& newKline);
        void publicTradeUpdated(const Tools::PublicTrades& newPublicTrades);

    };

}