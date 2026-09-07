#include "BybitOrderService.hpp"
#include "Tools/Network/Bybit/BybitRestAPI.hpp"

Core::Markets::BybitOrderService::BybitOrderService(QObject *parent)
    : BaseOrderService(std::make_unique<Tools::BybitRestAPI>(parent), parent)
{

}

void Core::Markets::BybitOrderService::setApi(const Tools::Api &api)
{

}

void Core::Markets::BybitOrderService::requestCreateOrder(const Tools::OrderRequest &request)
{

}

void Core::Markets::BybitOrderService::requestAmendOrder(const Tools::OrderAmendRequest &request)
{

}

void Core::Markets::BybitOrderService::requestCancelOrder(const Tools::OrderCancelRequest &request)
{

}

void Core::Markets::BybitOrderService::requestCancelAllOrders(const Tools::OrderCancelAllRequest &request)
{

}

void Core::Markets::BybitOrderService::requestOpenOrders(const Tools::OpenOrdersRequest &request)
{

}

void Core::Markets::BybitOrderService::requestOrderHistory(const Tools::OrderHistoryRequest &request)
{

}
