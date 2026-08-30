#pragma once
#include "WebSocketStreams.hpp"
#include <Tools/StdTypes.hpp>
#include <QObject>

namespace Core::Markets
{

    /*
     *  Класс для работы с потоковыми данными
    */
    class IMarketDataStreamer : public QObject
    {
        Q_OBJECT
    public:

        IMarketDataStreamer(QObject* parent = nullptr) : QObject(parent) {};
        virtual ~IMarketDataStreamer() = default;

        virtual void setApi(const Tools::Api& api) = 0;
        virtual void start() = 0;
        virtual void stop() = 0;
        virtual void restart() = 0;
        virtual bool hasRunned() = 0;

        // Подписка/отписка на потоки
        virtual void subscribeSymbol(const QString& symbol, QSet<WebSocketStreams> streams) = 0;
        virtual void unsubscribeSymbol(const QString& symbol, QSet<WebSocketStreams> streams) = 0;

        // Отправка всех текущих подписок (при переподключении)
        virtual void connectToStreams() = 0;
        virtual void disconnectFromStreams() = 0;

    protected:

        virtual void sendSubscriptionMessage(const QStringList& streams) = 0;
        virtual void sendUnsubscriptionMessage(const QStringList& streams) = 0;

        virtual QString createTickerStream(const QString& symbol) const = 0;
        virtual QString createOrderbookStream(const QString& symbol) const = 0;
        virtual QString createKlineStream(const QString& symbol) const = 0;
        virtual QString createPublicTradeStream(const QString& symbol) const = 0;

    signals:

        void started();
        void stopped();
        void pingMeasured(qint64 pingMs);
        void errorOccurred(const QString& error);

        void tickerUpdated(const Core::Tools::Ticker& newTicker);
        void orderbookUpdated(const Core::Tools::Orderbook& newOrderBook);
        void klineUpdated(const Core::Tools::Kline& newKline);
        void publicTradeUpdated(const Core::Tools::PublicTrades& newPublicTrades);

    };

}