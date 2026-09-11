#pragma once
#include "Markets/RestAPI/BasePrivateService.hpp"

namespace Core::Markets
{

class BybitPrivateService : public BasePrivateService
{
    Q_OBJECT
public:

    explicit BybitPrivateService(QObject* parent = nullptr);

    void init(const Tools::Api& api) override;

    void requestInfoAboutApi() override;
    void requestAccountBalance() override;
    void requestCreateOrder(const Tools::OrderRequest& request) override;
    void requestAmendOrder(const Tools::OrderAmendRequest& request) override;
    void requestCancelOrder(const Tools::OrderCancelRequest& request) override;
    void requestCancelAllOrders(const Tools::OrderCancelAllRequest& request) override;
    void requestOpenOrders(const Tools::OpenOrdersRequest& request) override;
    void requestOrderHistory(const Tools::OrderHistoryRequest& request) override;

};

}

