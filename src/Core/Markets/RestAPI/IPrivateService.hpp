#pragma once
#include "Tools/StdTypes.hpp"

namespace Core::Markets
{

class IPrivateService : public QObject
{
    Q_OBJECT
public:

    IPrivateService(QObject* parent = nullptr) : QObject(parent) {}

    virtual void init(const Tools::Api& api) = 0;

    // Запрос информации об аккаунте
    virtual void requestAccountBalance() = 0;
    // Запрос информации об Api
    virtual void requestInfoAboutApi() = 0;

    // ===================================================================================
    //  Управление ордерами
    // ===================================================================================

    // POST /v5/order/create — выставить новый ордер
    virtual void requestCreateOrder(const Tools::OrderRequest& request) = 0;
    // POST /v5/order/amend — изменить параметры еще не исполненного/частично исполненного ордера
    virtual void requestAmendOrder(const Tools::OrderAmendRequest& request) = 0;
    // POST /v5/order/cancel — отменить один конкретный ордер
    virtual void requestCancelOrder(const Tools::OrderCancelRequest& request) = 0;
    // POST /v5/order/cancel-all — отменить все ордера по фильтру
    virtual void requestCancelAllOrders(const Tools::OrderCancelAllRequest& request) = 0;

    // GET /v5/order/realtime — получить список активных ордеров
    virtual void requestOpenOrders(const Tools::OpenOrdersRequest& request) = 0;
    // GET /v5/order/history — получить историю закрытых/отмененных/исполненных ордеров
    virtual void requestOrderHistory(const Tools::OrderHistoryRequest& request) = 0;

signals:

    void accountBalanceReceived(const Core::Tools::AccountBalance& balance);
    void accountVerified();
    void infoAboutApiReceived(const Core::Tools::ApiInfo& apiInfo);

    // ===================================================================================
    //  Ответы на управление ордерами
    // ===================================================================================

    // Ордер успешно создан
    void orderCreated(const QString& orderId, const QString& orderLinkId);
    // Ордер успешно изменен
    void orderAmended(const QString& orderId, const QString& orderLinkId);
    // Ордер успешно отменен
    void orderCancelled(const QString& orderId, const QString& orderLinkId);
    // Массовая отмена выполнена — список ID отмененных ордеров
    void allOrdersCancelled(const QList<QString>& cancelledOrderIds);
    // Получен список активных ордеров
    void openOrdersReceived(const QList<Core::Tools::OrderInfo>& orders);
    // Получена история ордеров
    void orderHistoryReceived(const QList<Core::Tools::OrderInfo>& orders);

    // Отдельный канал для отказов конкретно по управлению ордерами
    void orderRequestRejected(const QString& orderLinkId, const QString& reason);
    void errorOccurred(const QString& error);
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

};

}
