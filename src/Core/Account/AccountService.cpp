#include "AccountService.hpp"
#include "Tools/OrderRequestBuilders.hpp"

Core::AccountService::AccountService(std::shared_ptr<AccountState> state, std::shared_ptr<AccountMediator> mediator, QObject *parent)
    : m_state(state), m_mediator(mediator), QObject(parent)
{
    connect(mediator.get(), &AccountMediator::apiReady,
            state.get(), &AccountState::updateApi);

    connect(mediator.get(), &AccountMediator::accountVerificationReady,
            state.get(), &AccountState::updateValidAccount);

    connect(mediator.get(), &AccountMediator::accountBalanceReady,
            state.get(), &AccountState::updateBalance);

    connect(mediator.get(), &AccountMediator::apiInfoReady,
            state.get(), &AccountState::updateApiInfo);

    connect(mediator.get(), &AccountMediator::walletReady,
            state.get(), &AccountState::updateBalance);

    connect(mediator.get(), &AccountMediator::orderReady,
            state.get(), &AccountState::updateOrder);

    connect(mediator.get(), &AccountMediator::executionReady,
            state.get(), &AccountState::updateExecution);

    connect(mediator.get(), &AccountMediator::positionReady,
            state.get(), &AccountState::updatePosition);

    connect(mediator.get(), &AccountMediator::messageSent,
            this, &AccountService::messageReceived);

    connect(mediator.get(), &AccountMediator::streamerStarted,
            this, &AccountService::onStreamerStarted);

    connect(mediator.get(), &AccountMediator::streamerStopped,
            this, &AccountService::onStreamerStopped);

    connect(mediator.get(), &AccountMediator::downloadProgress,
            this, &AccountService::downloadProgress);

    connect(mediator.get(), &AccountMediator::errorOccurredWithId,
            this, &AccountService::onErrorOccurredWithId);

    connect(mediator.get(), &AccountMediator::errorOccurred,
            this, &AccountService::errorOccurred);
}

void Core::AccountService::init(const Tools::Api &api)
{
    m_mediator->init(api);
}

void Core::AccountService::run()
{
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
    m_mediator->runStreamer();
}

void Core::AccountService::restart()
{
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
    m_mediator->restartStreamer();
}

void Core::AccountService::shutdown()
{
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

    m_mediator->stopStreamer();

    QEventLoop loop;
    QTimer::singleShot(5000, &loop, &QEventLoop::quit);

    connect(m_mediator.get(), &Core::AccountMediator::streamerStopped, &loop, &QEventLoop::quit, Qt::SingleShotConnection);

    loop.exec();
}

void Core::AccountService::subscribe()
{
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
    m_mediator->subscribe();
}

void Core::AccountService::onErrorOccurredWithId(const QString& id, const QString &error)
{
    QString errMsg = id + " : " + error;
    qCritical().noquote() << errMsg;
    emit errorOccurred(errMsg);
}

void Core::AccountService::onStreamerStarted(const QString& id)
{
    qDebug().noquote() << QString("%1 : started").arg(id);
    emit streamerStarted(id);
}

void Core::AccountService::onStreamerStopped(const QString& id)
{
    qDebug().noquote() << QString("%1 : stopped").arg(id);
    emit streamerStopped(id);
}

void Core::AccountService::createOrder(const QVariantMap& params)
{
    validateAndSend(params, Tools::buildOrderRequest,
                    [this](const Tools::OrderRequest& req) { m_mediator->requestCreateOrder(req); });
}

void Core::AccountService::amendOrder(const QVariantMap& params)
{
    validateAndSend(params, Tools::buildOrderAmendRequest,
                    [this](const Tools::OrderAmendRequest& req) { m_mediator->requestAmendOrder(req); });
}

void Core::AccountService::cancelOrder(const QVariantMap& params)
{
    validateAndSend(params, Tools::buildOrderCancelRequest,
                    [this](const Tools::OrderCancelRequest& req) { m_mediator->requestCancelOrder(req); });
}

void Core::AccountService::cancelAllOrders(const QVariantMap& params)
{
    validateAndSend(params, Tools::buildOrderCancelAllRequest,
                    [this](const Tools::OrderCancelAllRequest& req) { m_mediator->requestCancelAllOrders(req); });
}

void Core::AccountService::requestOpenOrders(const QVariantMap& params)
{
    validateAndSend(params, Tools::buildOpenOrdersRequest,
                    [this](const Tools::OpenOrdersRequest& req) { m_mediator->requestOpenOrders(req); });
}

void Core::AccountService::requestOrderHistory(const QVariantMap& params)
{
    validateAndSend(params, Tools::buildOrderHistoryRequest,
                    [this](const Tools::OrderHistoryRequest& req) { m_mediator->requestOrderHistory(req); });
}
