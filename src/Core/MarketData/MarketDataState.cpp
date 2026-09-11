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

}

