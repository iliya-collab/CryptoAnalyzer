#pragma once
#include "Markets/RestAPI/BaseOrderService.hpp"

namespace Core::Markets
{

class BybitOrderService : public BaseOrderService
{
    Q_OBJECT
public:

    explicit BybitOrderService(QObject* parent = nullptr);

    void setApi(const Tools::Api& api) override;

    void requestCreateOrder(const Tools::OrderRequest& request) override;
    void requestAmendOrder(const Tools::OrderAmendRequest& request) override;
    void requestCancelOrder(const Tools::OrderCancelRequest& request) override;
    void requestCancelAllOrders(const Tools::OrderCancelAllRequest& request) override;
    void requestOpenOrders(const Tools::OpenOrdersRequest& request) override;
    void requestOrderHistory(const Tools::OrderHistoryRequest& request) override;

};

}

