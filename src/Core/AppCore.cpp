#include "AppCore.hpp"
#include "Tools/LogManager.hpp"
#include "Config/ConfigurationManager.hpp"
#include <QVariant>
#include <QList>
#include <QVariantList>

AppCore::AppCore(QObject* parent) : QObject(parent)
{
    Core::Tools::LogManager::instance().setStdLogEnabled(false);
    Core::Tools::LogManager::instance().setLogFileEnabled(true);
    Core::Tools::LogManager::instance().setLogFile("app.log");

    m_marketState = std::make_shared<Core::MarketDataState>();
    m_marketMediator = std::make_shared<Core::MarketDataMediator>();
    m_marketService = std::make_shared<Core::MarketDataService>(m_marketState, m_marketMediator);

    m_accountMediator = std::make_shared<Core::AccountMediator>();
    m_accountState = std::make_shared<Core::AccountState>();
    m_accountService = std::make_shared<Core::AccountService>(m_accountState, m_accountMediator);
}

void AppCore::init()
{
    auto res = Core::ConfigurationManager::instance().load();

    if (!res.has_value())
    {
        qCritical() << res.error();
        qWarning() << "The application will use default configuration";
    }

    auto config = res.value();
    if (!config.m_apis.contains(config.m_activeApi))
    {
        qCritical() << "Active API not found:" << config.m_activeApi;
        return;
    }
    const auto& activeApi = config.m_apis.value(config.m_activeApi);

    m_accountMediator->init(activeApi);
    m_accountMediator->loadAccountBalance();

    m_marketMediator->init(activeApi.m_isTestnet);
    if (config.m_autoConnection)
        m_marketMediator->runStreamer();
}