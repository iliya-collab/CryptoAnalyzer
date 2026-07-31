#pragma once
#include "Core/MarketDataManager.hpp"
#include "Core/MarketDataStreamer.hpp"
#include "Core/ConfigurationManager.hpp"
#include "Core/Tools/DataModels/OrderbookSideModel.hpp"
#include "Core/Tools/DataModels/TradeModel.hpp"
#include "Core/Tools/DataModels/ReversedProxyModel.hpp"
#include <QQmlContext>
#include <QObject>
#include <QtQml>
#include <atomic>
#include <memory>

class AppCore : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(QVariantList tradeList MEMBER m_tradeList NOTIFY tradeListChanged FINAL)
    Q_PROPERTY(QVariantList candleSeries MEMBER m_candleSeries NOTIFY candleSeriesChanged FINAL)
    Q_PROPERTY(qint64 pingMs MEMBER m_pingMs NOTIFY pingMsChanged FINAL)

    Q_PROPERTY(Core::Tools::Ticker ticker READ getTicker NOTIFY tickerChanged FINAL)
    Q_PROPERTY(Core::Tools::API api READ getAPI NOTIFY apiChanged FINAL)
    Q_PROPERTY(Core::Tools::OrderbookSideModel* asks READ getAsks NOTIFY asksChanged FINAL)
    Q_PROPERTY(Core::Tools::OrderbookSideModel* bids READ getBids NOTIFY bidsChanged FINAL)
    Q_PROPERTY(Core::Tools::TradeModel* trades READ getTrades NOTIFY tradesChanged FINAL)

private:

    std::unique_ptr<Core::MarketDataManager> m_manager;
    std::unique_ptr<Core::MarketDataStreamer> m_streamer;

    QString m_lastTrade = "";
    QVariantList m_tradeList{};
    QVariantList m_candleSeries{};
    Core::Tools::API m_api{};
    Core::Tools::Ticker m_ticker{};
    std::unique_ptr<Core::Tools::OrderbookSideModel> m_asks{};
    std::unique_ptr<Core::Tools::OrderbookSideModel> m_bids{};
    std::unique_ptr<Core::Tools::TradeModel> m_trades{};
    std::atomic<bool> wasInit{false};
    qint64 m_pingMs = 0;

    void setupManagerConnections();
    void setupStreamerConnections();
    void setupConnections();

    void addCandle(const Core::ItemCandle& candle);
    void updateCandle(const Core::ItemCandle& candle);

public:

    explicit AppCore(QObject* parent = nullptr);
    ~AppCore();

    // Методы доступные в контексте QML
    Q_INVOKABLE void init();
    Q_INVOKABLE void run();
    Q_INVOKABLE void restart();
    Q_INVOKABLE void interrupt();
    Q_INVOKABLE void loadTradesFromRepository(const QString& category = "ALL");
    Q_INVOKABLE void loadTradesFromNetwork();
    Q_INVOKABLE void loadCandlesFromNetwork(const QString& symbol, const QString& interval, qint64 start, qint64 end);
    Q_INVOKABLE void setAPI(const QString& apiKey, const QString& secretKey, bool isTestnet);
    Q_INVOKABLE void saveAPI(const QString& apiKey, const QString& secretKey, bool isTestnet);
    Q_INVOKABLE void requestAccount();
    Q_INVOKABLE void addTrade(const QString& pair);

    // READ-методы
    Core::Tools::Ticker getTicker() const { return m_ticker; }
    Core::Tools::OrderbookSideModel* getAsks() const { return m_asks.get(); }
    Core::Tools::OrderbookSideModel* getBids() const { return m_bids.get(); }
    Core::Tools::TradeModel* getTrades() const { return m_trades.get(); }
    Core::Tools::API getAPI() const { return m_api; }

signals:

    // Сигналы для уведомления об измении свойств
    void tradeListChanged();
    void candleSeriesChanged();
    void apiChanged();
    void tickerChanged();
    void asksChanged();
    void bidsChanged();
    void pingMsChanged();
    void tradesChanged();

    // Сигналы работы самого приложения
    void errorOccurred(const QString& error);
    void messageReceived(const QString& msg);
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void started();
    void stopped();
};