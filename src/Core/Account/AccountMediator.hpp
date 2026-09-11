#pragma once
#include "Markets/RestAPI/IPrivateService.hpp"
#include "Markets/Websocket/IPrivateStreamer.hpp"
#include <QObject>

namespace Core {

class AccountMediator : public QObject
{
    Q_OBJECT
private:

    std::unique_ptr<Markets::IPrivateService> m_privateService;
    std::unique_ptr<Markets::IPrivateStreamer> m_privateStreamer;

public:

    explicit AccountMediator(QObject* parent = nullptr);

    void runStreamer();
    void stopStreamer();
    bool isStreamerRunning();
    void restartStreamer();
    void subscribe();

    void loadAccountBalance();
    void loadInfoAboutApi();

    void requestCreateOrder(const Tools::OrderRequest& request);
    void requestAmendOrder(const Tools::OrderAmendRequest& request);
    void requestCancelOrder(const Tools::OrderCancelRequest& request);
    void requestCancelAllOrders(const Tools::OrderCancelAllRequest& request);
    void requestOpenOrders(const Tools::OpenOrdersRequest& request);
    void requestOrderHistory(const Tools::OrderHistoryRequest& request);

    void init(const Tools::Api& api);

signals:

    // Уведомляет об ошибке
    void errorOccurredWithId(const QString& id, const QString& error);
    void errorOccurred(const QString& error);
    // Уведомляет об прогрессе загрузки запроса
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    // Уведомляет о старте потоковых данных
    void streamerStarted(const QString& id);
    // Уведомляет о остановке потоковых данных
    void streamerStopped(const QString& id);

    void messageSent(const QString& msg);

    // Уведомляет о получении данных
    void accountVerificationReady(bool isValid = true);
    void accountBalanceReady(const Core::Tools::AccountBalance& balance);
    void apiReady(const Core::Tools::Api& api);
    void apiInfoReady(const Core::Tools::ApiInfo& apiInfo);
    void orderCreated(const QString& orderId, const QString& orderLinkId);
    void orderAmended(const QString& orderId, const QString& orderLinkId);
    void orderCancelled(const QString& orderId, const QString& orderLinkId);
    void allOrdersCancelled(const QList<QString>& cancelledOrderIds);
    void openOrdersReady(const QList<Core::Tools::OrderInfo>& orders);
    void orderHistoryReady(const QList<Core::Tools::OrderInfo>& orders);
    void orderRequestRejected(const QString& orderLinkId, const QString& reason);

    void walletReady(const Core::Tools::AccountBalance& newAccountBalance);
    void orderReady(const Core::Tools::OrderInfo& order);
    void executionReady(const Core::Tools::ExecutionInfo& exec);
    void positionReady(const Core::Tools::PositionInfo& position);

private slots:

    void onAccountVerificationReady();
    void onAccountBalanceReady(const Core::Tools::AccountBalance& balance);
    void onInfoAboutApiReady(const Core::Tools::ApiInfo& apiInfo);
    void onOrderCreated(const QString& orderId, const QString& orderLinkId);
    void onOrderAmended(const QString& orderId, const QString& orderLinkId);
    void onOrderCancelled(const QString& orderId, const QString& orderLinkId);
    void onAllOrdersCancelled(const QList<QString>& cancelledOrderIds);
    void onOpenOrdersReceived(const QList<Core::Tools::OrderInfo>& orders);
    void onOrderHistoryReceived(const QList<Core::Tools::OrderInfo>& orders);
    void onOrderRequestRejected(const QString& orderLinkId, const QString& reason);

};

}