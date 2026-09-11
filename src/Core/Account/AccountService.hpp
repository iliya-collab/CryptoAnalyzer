#pragma once
#include "AccountMediator.hpp"
#include "AccountState.hpp"
#include <QObject>

namespace Core
{

    class AccountService : public QObject
    {
        Q_OBJECT

    private:

        std::shared_ptr<AccountState> m_state;
        std::shared_ptr<AccountMediator> m_mediator;

        template<typename BuiltFn, typename SendFn>
        void validateAndSend(const QVariantMap& params, BuiltFn&& build, SendFn&& send)
        {
            auto res = build(params);
            if (!res.has_value())
            {
                const QString orderLinkId = params.value("orderLinkId").toString();
                emit errorOccurred("orderLinkId : " + orderLinkId + " - " + res.error());
                return;
            }
            send(res.value());
        }

    public:

        explicit AccountService(std::shared_ptr<AccountState> state, std::shared_ptr<AccountMediator> mediator, QObject* parent = nullptr);

        Q_INVOKABLE void run();
        Q_INVOKABLE void restart();
        Q_INVOKABLE void shutdown();
        Q_INVOKABLE void init(const Tools::Api& api);
        Q_INVOKABLE void subscribe();

        Q_INVOKABLE void createOrder(const QVariantMap& params);
        Q_INVOKABLE void amendOrder(const QVariantMap& params);
        Q_INVOKABLE void cancelOrder(const QVariantMap& params);
        Q_INVOKABLE void cancelAllOrders(const QVariantMap& params);
        Q_INVOKABLE void requestOpenOrders(const QVariantMap& params);
        Q_INVOKABLE void requestOrderHistory(const QVariantMap& params);


    signals:

        // Уведомляет об ошибке
        void errorOccurred(const QString& error);
        // Уведомляет об прогрессе загрузки запроса
        void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
        // Уведомляет о старте потоковых данных
        void streamerStarted(const QString& id);
        // Уведомляет о остановке потоковых данных
        void streamerStopped(const QString& id);

        void messageReceived(const QString& msg);

    private slots:

        void onErrorOccurredWithId(const QString& id, const QString& error);
        void onStreamerStarted(const QString& id);
        void onStreamerStopped(const QString& id);

    };

}