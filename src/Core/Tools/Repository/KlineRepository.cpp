#include "KlineRepository.hpp"

namespace Core::Tools {

    KlineRepository::KlineRepository(const QString& dbPath, IDatabaseManager& manager) :
        BaseRepository(dbPath, manager) {}

    bool KlineRepository::init() {
        QStringList queries;

        queries << R"(
            CREATE TABLE IF NOT EXISTS candles (
                id INTEGER PRIMARY KEY,
                symbol TEXT NOT NULL,
                interval TEXT NOT NULL,
                start INTEGER NOT NULL,
                end INTEGER NOT NULL,
                open REAL,
                close REAL,
                high REAL,
                low REAL,
                volume REAL,
                turnover REAL,
                UNIQUE(symbol, interval, start, end)
            )
        )";

        queries << "CREATE INDEX IF NOT EXISTS idx_candles_start ON candles(start)";
        queries << "CREATE INDEX IF NOT EXISTS idx_candles_end ON candles(end)";
        queries << "CREATE INDEX IF NOT EXISTS idx_candles_interval ON candles(interval)";


        return m_dbManager.executeTransaction(m_dbPath, queries);
    }

    bool KlineRepository::open() {
        return m_dbManager.open(m_dbPath);
    }

    void KlineRepository::close() {
        m_dbManager.close(m_dbPath);
    }

    QString KlineRepository::error() {
        return m_dbManager.error();
    }

    bool KlineRepository::clear() {
        if (!m_dbManager.beginTransaction(m_dbPath))
            return false;

        if (!m_dbManager.executeQuery(m_dbPath, "DELETE FROM candles")) {
            m_dbManager.rollbackTransaction(m_dbPath);
            return false;
        }

        // if (!m_dbManager.executePrepared(m_dbPath, "UPDATE sqlite_sequence SET seq = 0 WHERE name = ?", {"candles"})) {
        //     m_dbManager.rollbackTransaction(m_dbPath);
        //     return false;
        // }

        return m_dbManager.commitTransaction(m_dbPath);
    }

    bool KlineRepository::insertKlines(const QList<Kline>& newKlines) {
        if (newKlines.isEmpty())
            return true;

        if (!m_dbManager.beginTransaction(m_dbPath))
            return false;

        for (const auto& item : newKlines)
            if (!m_dbManager.executePrepared(m_dbPath,
                R"(
                    INSERT INTO candles (symbol, interval, start, end, open, close, high, low, volume, turnover)
                    VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
                    ON CONFLICT(symbol, interval, start, end)
                    DO UPDATE SET
                        open = EXCLUDED.open,
                        close = EXCLUDED.close,
                        high = EXCLUDED.high,
                        low = EXCLUDED.low,
                        volume = EXCLUDED.volume,
                        turnover = EXCLUDED.turnover;
                )",
                { item.m_symbol, item.m_interval, item.m_start,
                    item.m_end, item.m_open, item.m_close,
                    item.m_high, item.m_low, item.m_volume, item.m_turnover }))
            {
                m_dbManager.rollbackTransaction(m_dbPath);
                return false;
            }

        return m_dbManager.commitTransaction(m_dbPath);
    }

    bool KlineRepository::insertKline(const Kline& newKline) {
        return m_dbManager.executePrepared(m_dbPath,
            R"(
                INSERT INTO candles (symbol, interval, start, end, open, close, high, low, volume, turnover)
                VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
                    ON CONFLICT(symbol, interval, start, end)
                    DO UPDATE SET
                        open = EXCLUDED.open,
                        close = EXCLUDED.close,
                        high = EXCLUDED.high,
                        low = EXCLUDED.low,
                        volume = EXCLUDED.volume,
                        turnover = EXCLUDED.turnover;
            )",
            { newKline.m_symbol, newKline.m_interval, newKline.m_start,
                newKline.m_end, newKline.m_open, newKline.m_close,
                newKline.m_high, newKline.m_low, newKline.m_volume, newKline.m_turnover });
    }
}
