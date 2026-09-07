#include "CryptoRepository.hpp"
#include <QDir>
#include <QFileInfo>

namespace Core::Tools {

    CryptoRepository::CryptoRepository(const QString& dbPath, IDatabaseManager& manager) :
        BaseRepository(dbPath, manager) {};

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

        // if (!m_dbManager.executePrepared(m_dbPath, "UPDATE sqlite_sequence SET seq = 0 WHERE name = ?", {"crypto"})) {
        //     m_dbManager.rollbackTransaction(m_dbPath);
        //     return false;
        // }

        return m_dbManager.commitTransaction(m_dbPath);
    }

    bool CryptoRepository::init() {
        QStringList queries;

        queries << R"(
            CREATE TABLE IF NOT EXISTS crypto (
                id INTEGER PRIMARY KEY,
                category TEXT UNIQUE NOT NULL,
                symbol TEXT UNIQUE NOT NULL,
                base_coin TEXT,
                quote_coin TEXT
            )
        )";

        queries << "CREATE INDEX IF NOT EXISTS idx_crypto_quote_coin ON crypto(quote_coin)";
        queries << "CREATE INDEX IF NOT EXISTS idx_crypto_category ON crypto(category)";

        return m_dbManager.executeTransaction(m_dbPath, queries);
    }

    bool CryptoRepository::open() {
        return m_dbManager.open(m_dbPath);
    }

    void CryptoRepository::close() {
        m_dbManager.close(m_dbPath);
    }

    QList<TradeInfo> CryptoRepository::getSelectedData() {
        return m_selectedData;
    }

    bool CryptoRepository::insertTrades(const QList<TradeInfo>& newTrades) {
        if (newTrades.isEmpty())
            return true;

        if (!m_dbManager.beginTransaction(m_dbPath))
            return false;

        for (const auto& item : newTrades)
            if (!m_dbManager.executePrepared(m_dbPath,
                    R"(INSERT INTO crypto
                        (category, symbol, base_coin, quote_coin)
                        VALUES (?, ?, ?, ?))",
                    { Tools::marketTypeToString(item.m_category), item.m_symbol, item.m_baseCoin, item.m_quoteCoin }))
            {
                m_dbManager.rollbackTransaction(m_dbPath);
                return false;
            }

        return m_dbManager.commitTransaction(m_dbPath);
    }

    void CryptoRepository::handleSelectedTrades(QSqlQuery& query) {
        while (query.next())
        {
            TradeInfo item;
            item.m_category = Tools::stringToMarketType(query.value(0).toString());
            item.m_symbol = query.value(1).toString();
            item.m_baseCoin = query.value(2).toString();
            item.m_quoteCoin = query.value(3).toString();
            m_selectedData.push_back(item);
        }
    }

    bool CryptoRepository::selectTrades(MarketType type) {
        m_selectedData.clear();
        return m_dbManager.executePrepared(m_dbPath,
            R"(SELECT category, symbol, base_coin, quote_coin
                FROM crypto WHERE category = ?)",
            {Tools::marketTypeToString(type)},
            [this](QSqlQuery& query)
        {
            handleSelectedTrades(query);
        });
    }

    bool CryptoRepository::selectTrades(MarketType type, const QString& quoteCoin) {
        m_selectedData.clear();
        return m_dbManager.executePrepared(m_dbPath,
           R"(SELECT category, symbol, base_coin, quote_coin
                FROM crypto WHERE category = ? AND quote_coin = ?)",
            {Tools::marketTypeToString(type), quoteCoin},
           [this](QSqlQuery& query)
        {
            handleSelectedTrades(query);
        });
    }

}
