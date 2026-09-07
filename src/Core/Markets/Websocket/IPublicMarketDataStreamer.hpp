#pragma once
#include "IMarketDataStreamer.hpp"
#include "Streams.hpp"
#include <QObject>

namespace Core::Markets
{

    class IPublicMarketDataStreamer : public IMarketDataStreamer
    {
        Q_OBJECT

    public:

        IPublicMarketDataStreamer(QObject* parent = nullptr) : IMarketDataStreamer(parent) {};
        virtual ~IPublicMarketDataStreamer() = default;

        // Отправка всех текущих подписок
        virtual void connectToStreams() = 0;
        virtual void disconnectFromStreams() = 0;

        // Подписка/отписка на потоки
        virtual void subscribeSymbol(const QString& symbol, QSet<PublicStreams> streams) = 0;
        virtual void unsubscribeSymbol(const QString& symbol, QSet<PublicStreams> streams) = 0;

    protected:

        virtual void sendSubscriptionMessage(const QStringList& streams) = 0;
        virtual void sendUnsubscriptionMessage(const QStringList& streams) = 0;

        virtual QString createTickerStream(const QString& symbol) const = 0;
        virtual QString createOrderbookStream(const QString& symbol) const = 0;
        virtual QString createKlineStream(const QString& symbol) const = 0;
        virtual QString createPublicTradeStream(const QString& symbol) const = 0;

    signals:

        void tickerUpdated(const Core::Tools::Ticker& newTicker);
        void orderbookUpdated(const Core::Tools::Orderbook& newOrderBook);
        void klineUpdated(const Core::Tools::Kline& newKline);
        void publicTradeUpdated(const Core::Tools::PublicTrades& newPublicTrades);

    };

}