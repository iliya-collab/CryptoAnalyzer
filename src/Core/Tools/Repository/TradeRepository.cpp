#include "TradeRepository.hpp"

namespace Core::Tools {

    TradeRepository::TradeRepository(const QString& dbPath, IDatabaseManager& manager) :
        BaseRepository(dbPath, manager) {}

    bool TradeRepository::init() {

        QString query = R"(
            CREATE TABLE IF NOT EXISTS trades (
                id INTEGER PRIMARY KEY,
                category TEXT NOT NULL,
                symbol TEXT NOT NULL,
                side TEXT NOT NULL,
                price REAL,
                volume REAL,
                time INTEGER
            )
        )";

        return m_dbManager.executeQuery(m_dbPath, query);
    }

    bool TradeRepository::open() {
        return m_dbManager.open(m_dbPath);
    }

    void TradeRepository::close() {
        m_dbManager.close(m_dbPath);
    }

    QString TradeRepository::error() {
        return m_dbManager.error();
    }

    bool TradeRepository::clear() {
        if (!m_dbManager.beginTransaction(m_dbPath))
            return false;

        if (!m_dbManager.executeQuery(m_dbPath, "DELETE FROM trades")) {
            m_dbManager.rollbackTransaction(m_dbPath);
            return false;
        }

        // if (!m_dbManager.executePrepared(m_dbPath, "UPDATE sqlite_sequence SET seq = 0 WHERE name = ?", {"trades"})) {
        //     m_dbManager.rollbackTransaction(m_dbPath);
        //     return false;
        // }

        return m_dbManager.commitTransaction(m_dbPath);
    }

}
