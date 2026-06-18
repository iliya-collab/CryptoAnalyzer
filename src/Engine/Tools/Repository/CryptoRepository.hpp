#pragma once

#include "../StdTypes.hpp"
#include "../Database/IDatabaseManager.hpp"

namespace Engine {

    class CryptoRepository {
    public:

        struct TradeInfo {
            QString symbol;
            QString base_coin;
            QString quote_coin;
        };

    private:

        QString m_dbPath;
        IDatabaseManager& m_dbManager;
        QList<TradeInfo> m_selectedData;

        void handleSelectedTrades(QSqlQuery& query);

    public:

        CryptoRepository(const QString& dbPath, IDatabaseManager& manager);
        ~CryptoRepository() = default;

        bool init();
        bool open();
        void close();
        QString error();
        bool clear();

        bool insertTrades(const QList<TradeInfo>& newTrades);
        bool selectTrades();
        bool selectTrades(const QString& quoteCoin);

        QList<TradeInfo> getSelectedData();

    };

}