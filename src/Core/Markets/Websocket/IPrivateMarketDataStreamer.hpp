#pragma once
#include "IMarketDataStreamer.hpp"
#include "Streams.hpp"
#include <QObject>

namespace Core::Markets
{

    class IPrivateMarketDataStreamer : public IMarketDataStreamer
    {
        Q_OBJECT

    public:

        IPrivateMarketDataStreamer(QObject* parent = nullptr) : IMarketDataStreamer(parent) {};
        virtual ~IPrivateMarketDataStreamer() = default;

        // Отправка всех текущих подписок
        virtual void connectToStreams() = 0;
        virtual void disconnectFromStreams() = 0;

        // Подписка/отписка на потоки
        virtual void subscribe(QSet<PrivateStreams> streams) = 0;
        virtual void unsubscribe(QSet<PrivateStreams> streams) = 0;

    protected:

        virtual void sendSubscriptionMessage(const QStringList& streams) = 0;
        virtual void sendUnsubscriptionMessage(const QStringList& streams) = 0;

        virtual QString createWalletStream() const = 0;
        virtual QString createOrderStream() const = 0;
        virtual QString createPositionStream() const = 0;
        virtual QString createExecutionStream() const = 0;

    signals:

        void walletUpdated(const Core::Tools::AccountBalance& newAccountBalance);
        void orderUpdated(const Core::Tools::OrderInfo& order);
        void executionUpdated(const Core::Tools::ExecutionInfo& exec);
        void positionUpdated(const Core::Tools::PositionInfo& position);

    };

}
