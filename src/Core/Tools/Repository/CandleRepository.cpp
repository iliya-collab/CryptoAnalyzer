#include "CandleRepository.hpp"

namespace Core::Tools {

    CandleRepository::CandleRepository(const QString& dbPath, IDatabaseManager& manager) :
        m_dbPath(dbPath), m_dbManager(manager) {}

    bool CandleRepository::init() {
        QStringList queries;

        queries << R"(
            CREATE TABLE IF NOT EXISTS candles (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                symbol TEXT NOT NULL,
                interval TEXT NOT NULL,
                start INTEGER NOT NULL,
                end INTEGER NOT NULL,
                open REAL,
                close REAL,
                high REAL,
                low REAL,
                UNIQUE(symbol, interval, start, end)
            )
        )";

        queries << "CREATE INDEX IF NOT EXISTS idx_candles_start ON candles(start)";
        queries << "CREATE INDEX IF NOT EXISTS idx_candles_end ON candles(end)";
        queries << "CREATE INDEX IF NOT EXISTS idx_candles_interval ON candles(interval)";


        return m_dbManager.executeTransaction(m_dbPath, queries);
    }

    bool CandleRepository::open() {
        return m_dbManager.open(m_dbPath);
    }

    void CandleRepository::close() {
        m_dbManager.close(m_dbPath);
    }

    QString CandleRepository::error() {
        return m_dbManager.error();
    }

    bool CandleRepository::clear() {
        if (!m_dbManager.beginTransaction(m_dbPath))
            return false;

        if (!m_dbManager.executeQuery(m_dbPath, "DELETE FROM candles")) {
            m_dbManager.rollbackTransaction(m_dbPath);
            return false;
        }

        if (!m_dbManager.executePrepared(m_dbPath, "UPDATE sqlite_sequence SET seq = 0 WHERE name = ?", {"candles"})) {
            m_dbManager.rollbackTransaction(m_dbPath);
            return false;
        }

        return m_dbManager.commitTransaction(m_dbPath);
    }

    bool CandleRepository::insertCandles(const QList<Kline>& newCandles) {
        if (newCandles.isEmpty())
            return true;

        if (!m_dbManager.beginTransaction(m_dbPath))
            return false;

        for (const auto& item : newCandles)
            if (!m_dbManager.executePrepared(m_dbPath,
                R"(
                    INSERT INTO candles (symbol, interval, start, end, open, close, high, low)
                    VALUES (?, ?, ?, ?, ?, ?, ?, ?)
                    ON CONFLICT(symbol, interval, start, end)
                    DO UPDATE SET
                        open = EXCLUDED.open,
                        close = EXCLUDED.close,
                        high = EXCLUDED.high,
                        low = EXCLUDED.low;
                )",
                { item.m_symbol, item.m_interval, item.m_start, item.m_end, item.m_open, item.m_close, item.m_high, item.m_low }))
            {
                m_dbManager.rollbackTransaction(m_dbPath);
                return false;
            }

        return m_dbManager.commitTransaction(m_dbPath);
    }

    bool CandleRepository::insertCandle(const Kline& newCandle) {
        return m_dbManager.executePrepared(m_dbPath,
            R"(
                INSERT INTO candles (symbol, interval, start, end, open, close, high, low)
                VALUES (?, ?, ?, ?, ?, ?, ?, ?)
                    ON CONFLICT(symbol, interval, start, end)
                    DO UPDATE SET
                        open = EXCLUDED.open,
                        close = EXCLUDED.close,
                        high = EXCLUDED.high,
                        low = EXCLUDED.low;
            )",
            { newCandle.m_symbol, newCandle.m_interval, newCandle.m_start, newCandle.m_end, newCandle.m_open, newCandle.m_close, newCandle.m_high, newCandle.m_low });
    }
}
