#pragma once
#include "Markets/Websocket/BasePrivateStreamer.hpp"

namespace Core::Markets
{

    class BybitPrivateStreamer : public BasePrivateStreamer {
        Q_OBJECT

    private slots:

        void onStarted() override;
        void onStopped() override;
        void onPingMeasured(qint64 pingMs) override;
        void onErrorOccurred(const QString& error) override;

    public:

        explicit BybitPrivateStreamer(QObject* parent = nullptr);
        ~BybitPrivateStreamer();
\
        QString id() override;
        void init(const Core::Tools::Api& api) override;
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