#include "AppCore.hpp"
#include "Tools/LogManager.hpp"
#include "ConfigurationManager.hpp"
#include "Markets/Bybit/BybitApiService.hpp"
#include "Markets/Bybit/BybitDataStreamer.hpp"
#include "Markets/Repository/MarketDataRepository.hpp"
#include <QVariant>
#include <QList>
#include <QVariantList>

AppCore::AppCore(QObject* parent) : QObject(parent)
{
    Core::Tools::LogManager::instance().setStdLogEnabled(false);
    Core::Tools::LogManager::instance().setLogFileEnabled(true);
    Core::Tools::LogManager::instance().setLogFile("app.log");

    m_apiService = std::make_unique<Core::Markets::BybitApiService>();
    m_streamer = std::make_unique<Core::Markets::BybitDataStreamer>();
    m_repository = std::make_unique<Core::Markets::MarketDataRepository>();

    m_marketState = std::make_shared<Core::MarketDataState>();
    m_marketMediator = std::make_shared<Core::MarketDataMediator>(std::move(m_apiService), std::move(m_streamer), std::move(m_repository), this);

    m_marketService = std::make_shared<Core::MarketDataService>(m_marketState, m_marketMediator);
}

void AppCore::init()
{
    auto res = Core::ConfigurationManager::instance().read();
    if (!res.has_value()) {
        qCritical() << res.error();
        qWarning() << "The application will use default configuration";
    }

    auto config = res.value();
    auto activeApi = config.m_ApiSet[config.m_activeApi];

    m_marketMediator->setApi(activeApi);
    m_marketMediator->loadAccountBalance();

    if (config.m_autoConnection)
        m_marketMediator->runStreamer();
}

void AppCore::saveApi(const QString& name, const Core::Tools::Api &api)
{
    Core::ConfigurationManager::instance().addApi(name, api);
}

void AppCore::saveConfig()
{
    Core::ConfigurationManager::instance().write();
}