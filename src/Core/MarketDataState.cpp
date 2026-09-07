#include "MarketDataState.hpp"

namespace Core {

    MarketDataState::MarketDataState(QObject *parent)
        : QObject{parent}
    {
        m_tradePairs = std::make_shared<Tools::TradePairsModel>();
        m_klineSeries = std::make_shared<Tools::KlineModel>();
        m_asks = std::make_shared<Tools::OrderbookSideModel>(Core::Tools::OrderbookSideModel::Side::Ask);
        m_bids = std::make_shared<Tools::OrderbookSideModel>(Core::Tools::OrderbookSideModel::Side::Bid);
        m_trades = std::make_shared<Tools::TradeModel>();
        m_assets = std::make_shared<Tools::AssetModel>();
        m_orders = std::make_shared<Tools::OrderModel>();
        m_executions = std::make_shared<Tools::ExecutionModel>();
        m_positions = std::make_shared<Tools::PositionModel>();
    }

    void MarketDataState::updateTradePairs(const QList<Tools::TradeInfo>& pairs)
    {
        m_tradePairs->updateTradePairs(pairs);
        emit tradePairsChanged();
    }

    void MarketDataState::updateKline(const Tools::Kline &kline)
    {
        m_klineSeries->updateKline(kline);
        emit klineSeriesChanged();
    }

    void MarketDataState::addHistoricalKlines(const QList<Tools::Kline>& klines)
    {
        m_klineSeries->addHistoricalKlines(klines);
    }

    void MarketDataState::updateTicker(const Tools::Ticker& ticker)
    {
        m_ticker = ticker;
        emit tickerChanged();
    }

    void MarketDataState::updateApi(const Tools::Api& api)
    {
        m_api = api;
        emit apiChanged();
    }

    void MarketDataState::updateApiInfo(const Tools::ApiInfo &apiInfo)
    {
        m_apiInfo = apiInfo;
        emit apiInfoChanged();
    }

    void MarketDataState::updateOrderbook(const Tools::Orderbook& orderbook)
    {
        m_asks->update(orderbook.m_asks);
        m_bids->update(orderbook.m_bids);
        emit asksChanged();
        emit bidsChanged();
    }

    void MarketDataState::updateTrades(const Tools::PublicTrades& trades)
    {
        m_trades->addTradeBatch(trades.m_items);
        emit tradesChanged();
    }

    void MarketDataState::updatePingMs(qint64 pingMs)
    {
        m_pingMs = pingMs;
        emit pingMsChanged();
    }

    void MarketDataState::updateValidAccount(bool isValid)
    {
        m_validAccount = isValid;
        emit validAccountChanged();
    }

    void MarketDataState::updateBalance(const Tools::AccountBalance &balance)
    {
        m_overallAssetsBalance = balance.m_totalWalletBalance;
        m_assets->updateAssets(balance.m_assets);

        emit overallAssetsBalanceChanged();
        emit assetsChanged();
    }

    void MarketDataState::updateOrder(const Tools::OrderInfo &order)
    {
        m_orders->upsertOrder(order);
        emit ordersChanged();
    }

    void MarketDataState::updateExecution(const Tools::ExecutionInfo &execution)
    {
        m_executions->addExecution(execution);
        emit executionsChanged();
    }

    void MarketDataState::updatePosition(const Tools::PositionInfo &position)
    {
        m_positions->upsertPosition(position);
        emit positionsChanged();
    }

}

