#pragma once
#include "Markets/Websocket/BasePublicMarketDataStreamer.hpp"
 
namespace Core::Markets
{

    class BybitPublicDataStreamer : public BasePublicMarketDataStreamer {
        Q_OBJECT

    private slots:

        void onStarted() override;
        void onStopped() override;
        void onPingMeasured(qint64 pingMs) override;
        void onErrorOccurred(const QString& error) override;

    public:

        explicit BybitPublicDataStreamer(Tools::MarketType type, QObject* parent = nullptr);
        ~BybitPublicDataStreamer();

        QString id() override;
        void setApi(const Core::Tools::Api& api) override;
        void start() override;
        void stop() override;
        void restart() override;
        bool isRunning() override;

    private:

        void sendSubscriptionMessage(const QStringList& streams) override;
        void sendUnsubscriptionMessage(const QStringList& streams) override;

        QString createTickerStream(const QString& symbol) const override;
        QString createOrderbookStream(const QString& symbol) const override;
        QString createKlineStream(const QString& symbol) const override;
        QString createPublicTradeStream(const QString& symbol) const override;

        QString m_lastPair = "";
        const int MAX_STREAMS_PER_SUBSCRIPTION = 10;

    };

}