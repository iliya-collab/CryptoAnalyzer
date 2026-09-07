#pragma once
#include "Markets/Websocket/BasePrivateMarketDataStreamer.hpp"

namespace Core::Markets
{

    class BybitPrivateDataStreamer : public BasePrivateMarketDataStreamer {
        Q_OBJECT

    private slots:

        void onStarted() override;
        void onStopped() override;
        void onPingMeasured(qint64 pingMs) override;
        void onErrorOccurred(const QString& error) override;

    public:

        explicit BybitPrivateDataStreamer(QObject* parent = nullptr);
        ~BybitPrivateDataStreamer();
\
        QString id() override;
        void setApi(const Core::Tools::Api& api) override;
        void start() override;
        void stop() override;
        void restart() override;
        bool isRunning() override;

    private:

        void sendSubscriptionMessage(const QStringList& streams) override;
        void sendUnsubscriptionMessage(const QStringList& streams) override;

        QString createWalletStream() const override;
        QString createOrderStream() const override;
        QString createPositionStream() const override;
        QString createExecutionStream() const override;

        const int MAX_STREAMS_PER_SUBSCRIPTION = 10;

    };

}