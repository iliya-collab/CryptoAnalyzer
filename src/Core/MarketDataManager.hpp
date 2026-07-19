#pragma once

#include <QObject>
#include <QList>
#include <QString>
#include <QMutex>
#include <QTimer>
#include "Tools/Network/BybitRestAPI.hpp"
#include "Tools/Repository/RepositoryCreater.hpp"

#define LOADING_TIMEOUT 30000

namespace Core {

    using ItemTrade = Tools::CryptoRepository::TradeInfo;
    using TradeList = QList<ItemTrade>;
    using ItemCandle = Tools::Kline;
    using CandleList = QList<ItemCandle>;

    class MarketDataManager : public QObject {
        Q_OBJECT
    private:


        mutable QMutex m_mutex;
        std::unique_ptr<Tools::CryptoRepository> m_cryptoRep;
        std::unique_ptr<Tools::CandleRepository> m_candleRep;
        Tools::BybitRestAPI* m_currentApi = nullptr;

        void processRequestTradePairs(TradeList& pairs, const QJsonObject& data);
        void processRequestCandles(CandleList& candles, const QString& interval, const QJsonObject& data);

    public:

        explicit MarketDataManager(QObject* parent = nullptr);
        ~MarketDataManager();

        void setAPI(const Tools::API& api);
        void init();

        // Работа с репозиториями
        // Загрузка всех данных из крипторепозитория
        TradeList loadAllFromCryptoRepository();
        // Загрузка данных из крипторепозитория по параметру quoteCoin
        TradeList loadFromCryptoRepository(const QString& quoteCoin);
        void clearCryptoRepository();
        // Сохранение данных в крипторепозитории
        void saveToCryptoRepository(const TradeList& tradePairs);
        void saveToCandleRepository(const ItemCandle& newCandle);
        void saveToCandlesRepository(const CandleList& newCandles);

        TradeList getTradeList() { return m_cryptoRep->getSelectedData(); }

        // Работа с сетью
        // Загрузка информации об аккаунте
        void requestInfoAboutAccount();
        // Загрузка споторых пар
        void requestTradePairs();
        // Загрузка свеч
        void requestCandles(const QString& symbol, const QString& interval, qint64 start, qint64 end);

    signals:

        void errorOccurred(const QString& error); // Уведомляет основной движок об ошибке
        void messageSent(const QString& msg); // Отправляет сообщение в движок
        void downloadProgress(qint64 bytesReceived, qint64 bytesTotal); // Уведомляет основной движок об загрузочном процессе
        void infoAboutAccountReceived(bool isValid);
        void candlesReceived(const CandleList& candles);
        void tradePairsReceived(const TradeList& pairs);

    };

} // namespace Engine