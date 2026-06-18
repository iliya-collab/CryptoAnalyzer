#include "CryptoRepository.hpp"
#include <QDir>
#include <QFileInfo>

namespace Engine {

    CryptoRepository::CryptoRepository(const QString& dbPath, IDatabaseManager& manager) :
        m_dbPath(dbPath), m_dbManager(manager) {};

    QString CryptoRepository::error() {
        return m_dbManager.error();
    }

    bool CryptoRepository::clear() {
        if (!m_dbManager.beginTransaction(m_dbPath))
            return false;

        if (!m_dbManager.executeQuery(m_dbPath, "DELETE FROM crypto")) {
            m_dbManager.rollbackTransaction(m_dbPath);
            return false;
        }

        if (!m_dbManager.executePrepared(m_dbPath, "UPDATE sqlite_sequence SET seq = 0 WHERE name = ?", {"crypto"})) {
            m_dbManager.rollbackTransaction(m_dbPath);
            return false;
        }

        return m_dbManager.commitTransaction(m_dbPath);
    }

    bool CryptoRepository::init() {
        QStringList queries;

        queries << R"(
            CREATE TABLE IF NOT EXISTS crypto (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                symbol TEXT UNIQUE NOT NULL,
                base_coin TEXT,
                quote_coin TEXT
            )
        )";

        queries << "CREATE INDEX IF NOT EXISTS idx_crypto_quote_coin ON crypto(quote_coin)";

        return m_dbManager.executeTransaction(m_dbPath, queries);
    }

    bool CryptoRepository::open() {
        return m_dbManager.open(m_dbPath);
    }

    void CryptoRepository::close() {
        m_dbManager.close(m_dbPath);
    }

    QList<CryptoRepository::TradeInfo> CryptoRepository::getSelectedData() {
        return m_selectedData;
    }

    bool CryptoRepository::insertTrades(const QList<TradeInfo>& newTrades) {
        if (newTrades.isEmpty())
            return true;

        if (!m_dbManager.beginTransaction(m_dbPath))
            return false;

        for (const auto& item : newTrades)
            if (!m_dbManager.executePrepared(m_dbPath, "INSERT INTO crypto (symbol, base_coin, quote_coin) VALUES (?, ?, ?)", { item.symbol, item.base_coin, item.quote_coin })) {
                m_dbManager.rollbackTransaction(m_dbPath);
                return false;
            }

        return m_dbManager.commitTransaction(m_dbPath);
    }

    void CryptoRepository::handleSelectedTrades(QSqlQuery& query) {
        while (query.next()) {
            TradeInfo item;
            item.symbol = query.value(0).toString();
            item.base_coin = query.value(1).toString();
            item.quote_coin = query.value(2).toString();
            m_selectedData.push_back(item);
        }
    }

    bool CryptoRepository::selectTrades() {
        return m_dbManager.executeQuery(m_dbPath, "SELECT symbol, base_coin, quote_coin FROM crypto", [this](QSqlQuery& query) {
            handleSelectedTrades(query);
        });
    }

    bool CryptoRepository::selectTrades(const QString& quoteCoin) {
        return m_dbManager.executePrepared(m_dbPath, "SELECT symbol, base_coin, quote_coin FROM crypto WHERE quote_coin = ?", {quoteCoin}, [this](QSqlQuery& query) {
            handleSelectedTrades(query);
        });
    }

}
