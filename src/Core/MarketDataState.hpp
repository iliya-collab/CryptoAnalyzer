#pragma once
#include "Tools/DataModels/TradePairsModel.hpp"
#include "Tools/DataModels/KlineModel.hpp"
#include "Tools/DataModels/OrderbookSideModel.hpp"
#include "Tools/DataModels/TradeModel.hpp"
#include "Tools/DataModels/AssetModel.hpp"
#include "Tools/DataModels/ReversedProxyModel.hpp"
#include "Tools/DataModels/TradePairsFilterProxyModel.hpp"
#include "Tools/DataModels/OrderModel.hpp"
#include "Tools/DataModels/ExecutionModel.hpp"
#include "Tools/DataModels/PositionModel.hpp"
#include <QObject>

namespace Core {

    class MarketDataState : public QObject
    {
        Q_OBJECT

        Q_PROPERTY(bool validAccount READ getValidAccount NOTIFY validAccountChanged FINAL)
        Q_PROPERTY(Core::Tools::TradePairsModel* tradePairs READ getTradePairs NOTIFY tradePairsChanged FINAL)
        Q_PROPERTY(Core::Tools::KlineModel* klineSeries READ getKlineSeries NOTIFY klineSeriesChanged FINAL)
        Q_PROPERTY(Core::Tools::AssetModel* assets READ getAssets NOTIFY assetsChanged FINAL)
        Q_PROPERTY(qint64 overallAssetsBalance READ getOverallAssetsBalance NOTIFY overallAssetsBalanceChanged FINAL)
        Q_PROPERTY(qint64 pingMs READ getPingMs NOTIFY pingMsChanged FINAL)
        Q_PROPERTY(Core::Tools::Ticker ticker READ getTicker NOTIFY tickerChanged FINAL)
        Q_PROPERTY(Core::Tools::Api api READ getApi NOTIFY apiChanged FINAL)
        Q_PROPERTY(Core::Tools::ApiInfo apiInfo READ getApiInfo NOTIFY apiInfoChanged FINAL)
        Q_PROPERTY(Core::Tools::OrderbookSideModel* asks READ getAsks NOTIFY asksChanged FINAL)
        Q_PROPERTY(Core::Tools::OrderbookSideModel* bids READ getBids NOTIFY bidsChanged FINAL)
        Q_PROPERTY(Core::Tools::TradeModel* trades READ getTrades NOTIFY tradesChanged FINAL)
        Q_PROPERTY(Core::Tools::OrderModel* orders READ getOrders NOTIFY ordersChanged FINAL)
        Q_PROPERTY(Core::Tools::ExecutionModel* executions READ getExecutions NOTIFY executionsChanged FINAL)
        Q_PROPERTY(Core::Tools::PositionModel* positions READ getPositions NOTIFY positionsChanged FINAL)

    private:

        // Даннные для аккаунта/пользователя
        bool m_validAccount = false;
        Tools::Api m_api{};
        Tools::ApiInfo m_apiInfo{};
        std::shared_ptr<Tools::AssetModel> m_assets{};
        qint64 m_overallAssetsBalance = 0;
        // Рыночные данные
        Tools::Ticker m_ticker{};
        std::shared_ptr<Tools::KlineModel> m_klineSeries{};
        std::shared_ptr<Tools::OrderbookSideModel> m_asks{};
        std::shared_ptr<Tools::OrderbookSideModel> m_bids{};
        std::shared_ptr<Tools::TradeModel> m_trades{};
        // Приватные данные
        std::shared_ptr<Tools::OrderModel> m_orders{};
        std::shared_ptr<Tools::ExecutionModel> m_executions{};
        std::shared_ptr<Tools::PositionModel> m_positions{};
        // Прочее
        std::shared_ptr<Tools::TradePairsModel> m_tradePairs{};
        qint64 m_pingMs = 0;

    public:

        explicit MarketDataState(QObject *parent = nullptr);

        // Методы обновления
        void updateTradePairs(const QList<Tools::TradeInfo>& pairs);
        void updateKline(const Tools::Kline& kline);
        void addHistoricalKlines(const QList<Tools::Kline>& klines);
        void updateTicker(const Tools::Ticker& ticker);
        void updateApi(const Tools::Api& api);
        void updateApiInfo(const Tools::ApiInfo& apiInfo);
        void updateOrderbook(const Tools::Orderbook& orderbook);
        void updateTrades(const Tools::PublicTrades& trades);
        void updatePingMs(qint64 pingMs);
        void updateValidAccount(bool isValid);
        void updateBalance(const Tools::AccountBalance& balance);
        void updateOrder(const Tools::OrderInfo& order);
        void updateExecution(const Tools::ExecutionInfo& execution);
        void updatePosition(const Tools::PositionInfo& position);

        // READ-методы
        bool getValidAccount() { return m_validAccount; }
        Tools::TradePairsModel* getTradePairs() const { return m_tradePairs.get(); }
        Tools::KlineModel* getKlineSeries() const { return m_klineSeries.get(); }
        Tools::AssetModel* getAssets() const { return m_assets.get(); }
        qint64 getPingMs() const { return m_pingMs; }
        Tools::Ticker getTicker() const { return m_ticker; }
        Tools::OrderbookSideModel* getAsks() const { return m_asks.get(); }
        Tools::OrderbookSideModel* getBids() const { return m_bids.get(); }
        Tools::TradeModel* getTrades() const { return m_trades.get(); }
        Tools::Api getApi() const { return m_api; }
        Tools::ApiInfo getApiInfo() const { return m_apiInfo; }
        qint64 getOverallAssetsBalance() const { return m_overallAssetsBalance; }
        Tools::OrderModel* getOrders() const { return m_orders.get(); }
        Tools::ExecutionModel* getExecutions() const { return m_executions.get(); }
        Tools::PositionModel* getPositions() const { return m_positions.get(); }

    signals:

        void validAccountChanged();
        void tradePairsChanged();
        void klineSeriesChanged();
        void assetsChanged();
        void apiChanged();
        void apiInfoChanged();
        void tickerChanged();
        void asksChanged();
        void bidsChanged();
        void pingMsChanged();
        void tradesChanged();
        void overallAssetsBalanceChanged();
        void ordersChanged();
        void executionsChanged();
        void positionsChanged();

    };

}
