#pragma once
#include "MarketData/MarketDataService.hpp"
#include "Account/AccountService.hpp"
#include <QQmlContext>
#include <QObject>
#include <memory>

class AppCore : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(Core::MarketDataState* marketState READ getMarketState CONSTANT)
    Q_PROPERTY(Core::MarketDataService* marketService READ getMarketService CONSTANT)
    Q_PROPERTY(Core::AccountState* accountState READ getAccountState CONSTANT)
    Q_PROPERTY(Core::AccountService* accountService READ getAccountService CONSTANT)

private:

    std::shared_ptr<Core::MarketDataMediator> m_marketMediator;
    std::shared_ptr<Core::MarketDataState> m_marketState;
    std::shared_ptr<Core::MarketDataService> m_marketService;

    std::shared_ptr<Core::AccountMediator> m_accountMediator;
    std::shared_ptr<Core::AccountState> m_accountState;
    std::shared_ptr<Core::AccountService> m_accountService;


public:

    explicit AppCore(QObject* parent = nullptr);
    ~AppCore() = default;

    Q_INVOKABLE void init();

    Core::MarketDataState* getMarketState() const { return m_marketState.get(); }
    Core::MarketDataService* getMarketService() const { return m_marketService.get(); }
    Core::AccountState* getAccountState() const { return m_accountState.get(); }
    Core::AccountService* getAccountService() const { return m_accountService.get(); }

};