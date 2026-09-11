#include "AccountState.hpp"

namespace Core {

    AccountState::AccountState(QObject *parent) : QObject{parent}
    {
        m_assets = std::make_shared<Tools::AssetModel>();
        m_orders = std::make_shared<Tools::OrderModel>();
        m_executions = std::make_shared<Tools::ExecutionModel>();
        m_positions = std::make_shared<Tools::PositionModel>();
    }


    void AccountState::updateApi(const Tools::Api& api)
    {
        m_api = api;
        emit apiChanged();
    }

    void AccountState::updateApiInfo(const Tools::ApiInfo &apiInfo)
    {
        m_apiInfo = apiInfo;
        emit apiInfoChanged();
    }

    void AccountState::updateValidAccount(bool isValid)
    {
        m_validAccount = isValid;
        emit validAccountChanged();
    }

    void AccountState::updateBalance(const Tools::AccountBalance &balance)
    {
        m_overallAssetsBalance = balance.m_totalWalletBalance;
        m_assets->updateAssets(balance.m_assets);

        emit overallAssetsBalanceChanged();
        emit assetsChanged();
    }

    void AccountState::updateOrder(const Tools::OrderInfo &order)
    {
        m_orders->upsertOrder(order);
        emit ordersChanged();
    }

    void AccountState::updateExecution(const Tools::ExecutionInfo &execution)
    {
        m_executions->addExecution(execution);
        emit executionsChanged();
    }

    void AccountState::updatePosition(const Tools::PositionInfo &position)
    {
        m_positions->upsertPosition(position);
        emit positionsChanged();
    }

}

