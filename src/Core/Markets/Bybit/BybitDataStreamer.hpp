#pragma once
#include "Markets/BaseMarketDataStreamer.hpp"
 
namespace Core::Markets
{

/*
 *  Класс для работы с потоковыми данными (ByBit Websocket API)
*/
    class BybitDataStreamer : public BaseMarketDataStreamer {
        Q_OBJECT

    private slots:

        void onStarted() override;
        void onStopped() override;
        void onPingMeasured(qint64 pingMs) override;
        void onErrorOccurred(const QString& error) override;
        void onMessageReceived(const QJsonObject& message) override;

    public:

        explicit BybitDataStreamer(QObject* parent = nullptr);
        ~BybitDataStreamer();

        void setApi(const Tools::Api& api) override;
        void start() override;
        void stop() override;
        void restart() override;
        bool hasRunned() override;

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