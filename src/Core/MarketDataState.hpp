#pragma once
#include "Tools/DataModels/TradePairsModel.hpp"
#include "Tools/DataModels/KlineModel.hpp"
#include "Tools/DataModels/OrderbookSideModel.hpp"
#include "Tools/DataModels/TradeModel.hpp"
#include "Tools/DataModels/ReversedProxyModel.hpp"
#include "Tools/DataModels/TradePairsFilterProxyModel.hpp"
#include <QObject>

namespace Core {

    class MarketDataState : public QObject {
        Q_OBJECT

        Q_PROPERTY(bool validAccount READ getValidAccount NOTIFY validAccountChanged FINAL)
        Q_PROPERTY(Tools::TradePairsModel* tradePairs READ getTradePairs NOTIFY tradePairsChanged FINAL)
        Q_PROPERTY(Tools::KlineModel* klineSeries READ getKlineSeries NOTIFY klineSeriesChanged FINAL)
        Q_PROPERTY(qint64 pingMs READ getPingMs NOTIFY pingMsChanged FINAL)
        Q_PROPERTY(Tools::Ticker ticker READ getTicker NOTIFY tickerChanged FINAL)
        Q_PROPERTY(Tools::API api READ getAPI NOTIFY apiChanged FINAL)
        Q_PROPERTY(Tools::OrderbookSideModel* asks READ getAsks NOTIFY asksChanged FINAL)
        Q_PROPERTY(Tools::OrderbookSideModel* bids READ getBids NOTIFY bidsChanged FINAL)
        Q_PROPERTY(Tools::TradeModel* trades READ getTrades NOTIFY tradesChanged FINAL)

    private:

        bool m_validAccount;
        Tools::API m_api{};
        Tools::Ticker m_ticker{};
        std::shared_ptr<Tools::KlineModel> m_klineSeries{};
        std::shared_ptr<Tools::TradePairsModel> m_tradePairs{};
        std::shared_ptr<Tools::OrderbookSideModel> m_asks{};
        std::shared_ptr<Tools::OrderbookSideModel> m_bids{};
        std::shared_ptr<Tools::TradeModel> m_trades{};
        qint64 m_pingMs = 0;

    public:

        explicit MarketDataState(QObject *parent = nullptr);

        // Методы обновления
        void updateTradePairs(const QList<Tools::TradeInfo>& pairs);
        void updateKline(const Tools::Kline& kline);
        void addHistoricalKlines(const QList<Tools::Kline>& klines);
        void updateTicker(const Tools::Ticker& ticker);
        void updateAPI(const Tools::API& api);
        void updateOrderbook(const Tools::Orderbook& orderbook);
        void updateTrades(const Tools::PublicTrades& trades);
        void updatePingMs(qint64 pingMs);
        void updateValidAccount(bool isValid);

        // READ-методы
        bool getValidAccount() { return m_validAccount; }
        Tools::TradePairsModel* getTradePairs() const { return m_tradePairs.get(); }
        Tools::KlineModel* getKlineSeries() const { return m_klineSeries.get(); }
        qint64 getPingMs() const { return m_pingMs; }
        Tools::Ticker getTicker() const { return m_ticker; }
        Tools::OrderbookSideModel* getAsks() const { return m_asks.get(); }
        Tools::OrderbookSideModel* getBids() const { return m_bids.get(); }
        Tools::TradeModel* getTrades() const { return m_trades.get(); }
        Tools::API getAPI() const { return m_api; }

    signals:

        void validAccountChanged();
        void tradePairsChanged();
        void klineSeriesChanged();
        void apiChanged();
        void tickerChanged();
        void asksChanged();
        void bidsChanged();
        void pingMsChanged();
        void tradesChanged();

    };

}
