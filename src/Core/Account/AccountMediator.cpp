#include "AccountMediator.hpp"
#include "Markets/Bybit/BybitPrivateService.hpp"
#include "Markets/Bybit/BybitPrivateStreamer.hpp"

namespace Core {

AccountMediator::AccountMediator(QObject* parent) : QObject(parent)
{
    m_privateService = std::make_unique<Core::Markets::BybitPrivateService>();
    m_privateStreamer = std::make_unique<Core::Markets::BybitPrivateStreamer>();

    // Связываем Сеть (REST)
    connect(m_privateService.get(), &Markets::IPrivateService::accountVerified,
            this, &AccountMediator::onAccountVerificationReady);

    connect(m_privateService.get(), &Markets::IPrivateService::accountBalanceReceived,
            this, &AccountMediator::onAccountBalanceReady);

    connect(m_privateService.get(), &Markets::IPrivateService::orderCreated,
            this, &AccountMediator::onOrderCreated);

    connect(m_privateService.get(), &Markets::IPrivateService::orderAmended,
            this, &AccountMediator::onOrderAmended);

    connect(m_privateService.get(), &Markets::IPrivateService::orderCancelled,
            this, &AccountMediator::onOrderCancelled);

    connect(m_privateService.get(), &Markets::IPrivateService::allOrdersCancelled,
            this, &AccountMediator::onAllOrdersCancelled);

    connect(m_privateService.get(), &Markets::IPrivateService::openOrdersReceived,
            this, &AccountMediator::onOpenOrdersReceived);

    connect(m_privateService.get(), &Markets::IPrivateService::orderHistoryReceived,
            this, &AccountMediator::onOrderHistoryReceived);

    connect(m_privateService.get(), &Markets::IPrivateService::orderRequestRejected,
            this, &AccountMediator::onOrderRequestRejected);

    // Связываем приватный стрим
    connect(m_privateStreamer.get(), &Markets::IPrivateStreamer::errorOccurred,
            this, &AccountMediator::errorOccurredWithId);

    connect(m_privateStreamer.get(), &Markets::IPrivateStreamer::started,
            this, &AccountMediator::streamerStarted);

    connect(m_privateStreamer.get(), &Markets::IPrivateStreamer::stopped,
            this, &AccountMediator::streamerStopped);

    connect(m_privateStreamer.get(), &Markets::IPrivateStreamer::orderUpdated,
            this, &AccountMediator::orderReady);

    connect(m_privateStreamer.get(), &Markets::IPrivateStreamer::executionUpdated,
            this, &AccountMediator::executionReady);

    connect(m_privateStreamer.get(), &Markets::IPrivateStreamer::positionUpdated,
            this, &AccountMediator::positionReady);

    connect(m_privateStreamer.get(), &Markets::IPrivateStreamer::walletUpdated,
            this, &AccountMediator::walletReady);

}

void AccountMediator::runStreamer()
{
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
    m_privateStreamer->start();
}

void AccountMediator::stopStreamer()
{
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
    m_privateStreamer->stop();
}

bool AccountMediator::isStreamerRunning()
{
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
    return m_privateStreamer->isRunning();
}

void AccountMediator::restartStreamer()
{
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
    m_privateStreamer->restart();
}


void AccountMediator::subscribe()
{
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
    m_privateStreamer->subscribe({
        Markets::PrivateStreams::Wallet,
        Markets::PrivateStreams::Order,
        Markets::PrivateStreams::Execution,
        Markets::PrivateStreams::Position
    });
}

void AccountMediator::loadAccountBalance()
{
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
    emit messageSent("Loading account balance...");
    m_privateService->requestAccountBalance();
}

void AccountMediator::loadInfoAboutApi()
{
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
    emit messageSent("Loading information about your API...");
    m_privateService->requestInfoAboutApi();
}

void AccountMediator::requestCreateOrder(const Tools::OrderRequest &request)
{
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
    emit messageSent("Creating order...");
    m_privateService->requestCreateOrder(request);
}

void AccountMediator::requestAmendOrder(const Tools::OrderAmendRequest &request)
{
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
    emit messageSent("Amending order...");
    m_privateService->requestAmendOrder(request);
}

void AccountMediator::requestCancelOrder(const Tools::OrderCancelRequest &request)
{
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
    emit messageSent("Order cancellation...");
    m_privateService->requestCancelOrder(request);
}

void AccountMediator::requestCancelAllOrders(const Tools::OrderCancelAllRequest &request)
{
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
    emit messageSent("Cancellation of all orders...");
    m_privateService->requestCancelAllOrders(request);
}

void AccountMediator::requestOpenOrders(const Tools::OpenOrdersRequest &request)
{
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
    emit messageSent("Loading open orders...");
    m_privateService->requestOpenOrders(request);
}

void AccountMediator::requestOrderHistory(const Tools::OrderHistoryRequest &request)
{
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
    emit messageSent("Loading history orders...");
    m_privateService->requestOrderHistory(request);
}

void AccountMediator::init(const Tools::Api &api)
{
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
    m_privateService->init(api);
    m_privateStreamer->init(api);
    emit apiReady(api);
}

void AccountMediator::onAccountVerificationReady()
{
    emit messageSent("Account verified");
    emit accountVerificationReady();
}

void AccountMediator::onAccountBalanceReady(const Core::Tools::AccountBalance& balance)
{
    emit messageSent("Account balance ready");
    emit accountBalanceReady(balance);
}

void AccountMediator::onInfoAboutApiReady(const Tools::ApiInfo &apiInfo)
{
    emit messageSent("The information about your API ready");
    emit apiInfoReady(apiInfo);
}

void AccountMediator::onOrderCreated(const QString &orderId, const QString &orderLinkId)
{
    emit messageSent("The order [" + orderLinkId + "] was created");
    emit orderCreated(orderId, orderLinkId);
}

void AccountMediator::onOrderAmended(const QString &orderId, const QString &orderLinkId)
{
    emit messageSent("The order [" + orderLinkId + "] was amended");
    emit orderAmended(orderId, orderLinkId);
}

void AccountMediator::onOrderCancelled(const QString &orderId, const QString &orderLinkId)
{
    emit messageSent("The order [" + orderLinkId + "] was cancelled");
    emit orderCancelled(orderId, orderLinkId);
}

void AccountMediator::onAllOrdersCancelled(const QList<QString> &cancelledOrderIds)
{
    emit messageSent("All orders were cancelled");
    emit allOrdersCancelled(cancelledOrderIds);
}

void AccountMediator::onOrderRequestRejected(const QString &orderLinkId, const QString &reason)
{
    emit errorOccurred("The order [" + orderLinkId + "] was rejected. Reason : " + reason);
    emit orderRequestRejected(orderLinkId, reason);
}

void AccountMediator::onOrderHistoryReceived(const QList<Tools::OrderInfo> &orders)
{
    emit messageSent("Order history was received");
    emit orderHistoryReady(orders);
}

void AccountMediator::onOpenOrdersReceived(const QList<Tools::OrderInfo> &orders)
{
    emit messageSent("Open orders were received");
    emit openOrdersReady(orders);
}

}