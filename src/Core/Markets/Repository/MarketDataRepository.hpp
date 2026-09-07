#pragma once
#include <QObject>
#include "Tools/Repository/TradeRepository.hpp"
#include "Tools/Repository/KlineRepository.hpp"
#include "Tools/Repository/CryptoRepository.hpp"

namespace Core::Markets {

/*
 *  Класс для работы с локальным репозиторием
*/
    class MarketDataRepository : public QObject {
        Q_OBJECT
    private:

        std::unique_ptr<Tools::CryptoRepository> m_cryptoRep;
        std::unique_ptr<Tools::KlineRepository> m_klineRep;
        std::unique_ptr<Tools::TradeRepository> m_tradeRep;
        mutable QMutex m_mutex;

    public:

        explicit MarketDataRepository(QObject* parent = nullptr);

        // Загрузка всех данных из крипторепозитория
        QList<Tools::TradeInfo> loadAllFromCryptoRepository(Tools::MarketType type);
        void clearCryptoRepository();

        // Сохранение данных в крипторепозитории
        void saveToCryptoRepository(const QList<Tools::TradeInfo>& tradePairs);
        void saveToKlineRepository(const Tools::Kline& newKline);
        void saveToKlinesRepository(const QList<Tools::Kline>& newKlines);

        QList<Tools::TradeInfo> getTradeList() { return m_cryptoRep->getSelectedData(); }

    signals:

        void errorOccurred(const QString& error);

    };

}

