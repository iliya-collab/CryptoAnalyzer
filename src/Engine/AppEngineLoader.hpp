#pragma once

#include <QObject>
#include <QList>
#include <QString>
#include <QMutex>
#include <QTimer>
#include "Tools/Network/BybitRestAPI.hpp"
#include "Tools/Repository/RepositoryCreater.hpp"

#define LOADING_TIMEOUT 30000

namespace Engine {

    class AppEngineLoader : public QObject {
        Q_OBJECT
    private:

        using TradeInfo = CryptoRepository::TradeInfo;
        using TradeList = QList<CryptoRepository::TradeInfo>;

        mutable QMutex m_mutex;
        std::unique_ptr<CryptoRepository> m_cryptoRep;
        std::unique_ptr<CandleRepository> m_candleRep;
        BybitRestAPI* m_currentApi = nullptr;

        void processRequestTradePairs(TradeList& pairs, const QJsonObject& data);
        void processRequestCandles(QList<Kline>& candles, const QJsonObject& data);

    public:

        explicit AppEngineLoader(QObject* parent = nullptr);
        ~AppEngineLoader();

        void setAPI(const API& api);
        void init();

        // Работа с репозиториями
        // Загрузка всех данных из крипторепозитория
        TradeList loadAllFromCryptoRepository();
        // Загрузка данных из крипторепозитория по параметру quoteCoin
        TradeList loadFromCryptoRepository(const QString& quoteCoin);
        void clearCryptoRepository();
        // Сохранение данных в крипторепозитории
        void saveToCryptoRepository(const TradeList& tradePairs);
        void saveToCandleRepository(const Kline& newCandle);
        void saveToCandlesRepository(const QList<Kline>& newCandles);

        // Работа с сетью
        // Загрузка информации об аккаунте
        void requestInfoAboutAccount();
        // Загрузка споторых пар
        void requestTradePairs();
        // Загрузка свеч
        void requestCandles(const QString& symbol, const QString& interval, int start, int end);

    signals:

        void errorOccurred(const QString& error); // Уведомляет основной движок об ошибке
        void messageSent(const QString& msg); // Отправляет сообщение в движок
        void downloadProgress(qint64 bytesReceived, qint64 bytesTotal); // Уведомляет основной движок об загрузочном процессе
        void infoAboutAccountReceived(bool isValid);
        void candlesReceived(const QList<Kline>& candles);
        void tradePairsReceived(const TradeList& pairs);

    };

} // namespace Engine