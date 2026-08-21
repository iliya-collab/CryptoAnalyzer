#pragma once
#include "BaseRepository.hpp"
#include "../StdTypes.hpp"
#include "../Database/IDatabaseManager.hpp"

namespace Core::Tools {

    class CryptoRepository : public BaseRepository {
    private:

        QList<TradeInfo> m_selectedData;

        void handleSelectedTrades(QSqlQuery& query);

    public:

        CryptoRepository(const QString& dbPath, IDatabaseManager& manager);
        ~CryptoRepository() = default;

        bool init() override;
        bool open() override;
        void close() override;
        QString error() override;
        bool clear() override;

        bool insertTrades(const QList<TradeInfo>& newTrades);
        bool selectTrades();
        bool selectTrades(const QString& quoteCoin);

        QList<TradeInfo> getSelectedData();

    };

}