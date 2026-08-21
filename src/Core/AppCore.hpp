#pragma once
#include "MarketDataService.hpp"
#include <QQmlContext>
#include <QObject>
#include <memory>

class AppCore : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(Core::MarketDataState* marketState READ getMarketState CONSTANT)
    Q_PROPERTY(Core::MarketDataService* marketService READ getMarketService CONSTANT)

private:

    std::unique_ptr<Core::Markets::BaseMarketApiService> m_apiService;
    std::unique_ptr<Core::Markets::IMarketDataStreamer> m_streamer;
    std::unique_ptr<Core::Markets::MarketDataRepository> m_repository;

    std::shared_ptr<Core::MarketDataMediator> m_marketMediator;
    std::shared_ptr<Core::MarketDataState> m_marketState;
    std::shared_ptr<Core::MarketDataService> m_marketService;

public:

    explicit AppCore(QObject* parent = nullptr);
    ~AppCore() = default;

    Q_INVOKABLE void init();

    Q_INVOKABLE void saveAPI(const QString& name, const Core::Tools::API& api);
    Q_INVOKABLE void saveConfig();

    Core::MarketDataState* getMarketState() const { return m_marketState.get(); }
    Core::MarketDataService* getMarketService() const { return m_marketService.get(); }

};