#include "BybitPrivateService.hpp"
#include "Tools/Network/Bybit/BybitRestAPI.hpp"
#include "Handlers/RestAPI/GetBybitAccountBalanceHandler.hpp"
#include "Handlers/RestAPI/GetBybitInfoAboutAPIHandler.hpp"
#include "Handlers/RestAPI/PostBybitCreateOrderHandler.hpp"
#include "Handlers/RestAPI/PostBybitAmendOrderHandler.hpp"
#include "Handlers/RestAPI/PostBybitCancelOrderHandler.hpp"
#include "Handlers/RestAPI/PostBybitCancelAllOrdersHandler.hpp"

Core::Markets::BybitPrivateService::BybitPrivateService(QObject *parent)
    : BasePrivateService(std::make_unique<Tools::BybitRestAPI>(parent), parent)
{
    registerHandler<GetBybitInfoAboutAPIHandler>();
    registerHandler<GetBybitAccountBalanceHandler>();
    registerHandler<PostBybitCreateOrderHandler>();
    registerHandler<PostBybitAmendOrderHandler>();
    registerHandler<PostBybitCancelOrderHandler>();
    registerHandler<PostBybitCancelAllOrdersHandler>();
}

void Core::Markets::BybitPrivateService::requestInfoAboutApi()
{
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

    send<GetBybitInfoAboutAPIHandler>();
}

void Core::Markets::BybitPrivateService::requestAccountBalance()
{
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

    send<GetBybitAccountBalanceHandler>();
}

void Core::Markets::BybitPrivateService::init(const Tools::Api &api)
{
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

    if (!m_currentApi)
        return;

    m_currentApi->init(api);
}

void Core::Markets::BybitPrivateService::requestCreateOrder(const Tools::OrderRequest &request)
{
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

    if (!m_currentApi)
        return;

    send<PostBybitCreateOrderHandler>(request);
}

void Core::Markets::BybitPrivateService::requestAmendOrder(const Tools::OrderAmendRequest &request)
{
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

    if (!m_currentApi)
        return;

    send<PostBybitAmendOrderHandler>(request);
}

void Core::Markets::BybitPrivateService::requestCancelOrder(const Tools::OrderCancelRequest &request)
{
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

    if (!m_currentApi)
        return;

    send<PostBybitCancelOrderHandler>(request);
}

void Core::Markets::BybitPrivateService::requestCancelAllOrders(const Tools::OrderCancelAllRequest &request)
{
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

    if (!m_currentApi)
        return;

    send<PostBybitCancelAllOrdersHandler>(request);
}

void Core::Markets::BybitPrivateService::requestOpenOrders(const Tools::OpenOrdersRequest &request)
{
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

    if (!m_currentApi)
        return;
}

void Core::Markets::BybitPrivateService::requestOrderHistory(const Tools::OrderHistoryRequest &request)
{
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

    if (!m_currentApi)
        return;
}
