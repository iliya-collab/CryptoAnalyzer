#pragma once
#include "Tools/DataModels/AssetModel.hpp"
#include "Tools/DataModels/OrderModel.hpp"
#include "Tools/DataModels/ExecutionModel.hpp"
#include "Tools/DataModels/PositionModel.hpp"
#include <QObject>

namespace Core {

class AccountState : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool validAccount READ getValidAccount NOTIFY validAccountChanged FINAL)
    Q_PROPERTY(Core::Tools::AssetModel* assets READ getAssets NOTIFY assetsChanged FINAL)
    Q_PROPERTY(qint64 overallAssetsBalance READ getOverallAssetsBalance NOTIFY overallAssetsBalanceChanged FINAL)
    Q_PROPERTY(Core::Tools::Api api READ getApi NOTIFY apiChanged FINAL)
    Q_PROPERTY(Core::Tools::ApiInfo apiInfo READ getApiInfo NOTIFY apiInfoChanged FINAL)
    Q_PROPERTY(Core::Tools::OrderModel* orders READ getOrders NOTIFY ordersChanged FINAL)
    Q_PROPERTY(Core::Tools::ExecutionModel* executions READ getExecutions NOTIFY executionsChanged FINAL)
    Q_PROPERTY(Core::Tools::PositionModel* positions READ getPositions NOTIFY positionsChanged FINAL)

private:

    // Даннные аккаунта/пользователя
    bool m_validAccount = false;
    Tools::Api m_api{};
    Tools::ApiInfo m_apiInfo{};
    std::shared_ptr<Tools::AssetModel> m_assets{};
    qint64 m_overallAssetsBalance = 0;
    std::shared_ptr<Tools::OrderModel> m_orders{};
    std::shared_ptr<Tools::ExecutionModel> m_executions{};
    std::shared_ptr<Tools::PositionModel> m_positions{};

public:

    explicit AccountState(QObject *parent = nullptr);

    // Методы обновления
    void updateApi(const Tools::Api& api);
    void updateApiInfo(const Tools::ApiInfo& apiInfo);
    void updateValidAccount(bool isValid);
    void updateBalance(const Tools::AccountBalance& balance);
    void updateOrder(const Tools::OrderInfo& order);
    void updateExecution(const Tools::ExecutionInfo& execution);
    void updatePosition(const Tools::PositionInfo& position);

    // READ-методы
    bool getValidAccount() { return m_validAccount; }
    Tools::AssetModel* getAssets() const { return m_assets.get(); }
    Tools::Api getApi() const { return m_api; }
    Tools::ApiInfo getApiInfo() const { return m_apiInfo; }
    qint64 getOverallAssetsBalance() const { return m_overallAssetsBalance; }
    Tools::OrderModel* getOrders() const { return m_orders.get(); }
    Tools::ExecutionModel* getExecutions() const { return m_executions.get(); }
    Tools::PositionModel* getPositions() const { return m_positions.get(); }

signals:

    void validAccountChanged();
    void assetsChanged();
    void apiChanged();
    void apiInfoChanged();
    void overallAssetsBalanceChanged();
    void ordersChanged();
    void executionsChanged();
    void positionsChanged();

};

}
