#pragma once

#include "../StdTypes.hpp"
#include "../Database/IDatabaseManager.hpp"

namespace Engine {

    class CandleRepository {
    private:

        QString m_dbPath;
        IDatabaseManager& m_dbManager;

    public:

        CandleRepository(const QString& dbPath, IDatabaseManager& manager);
        ~CandleRepository() = default;

        bool init();
        bool open();
        void close();
        QString error();
        bool clear();

        bool insertCandles(const QList<Kline>& newCandles);
        bool insertCandle(const Kline& newCandle);

    };

}