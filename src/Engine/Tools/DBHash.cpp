#include "Engine/Tools/DBHash.hpp"
#include "Engine/Managers/DataBaseManager.hpp"

namespace Engine {

    bool DBHash::dbExist() {
        auto& db_manager = DataBaseManager::instance();
        return db_manager.existDBFile(m_crypto_db);
    }

    bool DBHash::create() {
        auto& db_manager = DataBaseManager::instance();

        if (!db_manager.open(m_crypto_db)) {
            m_last_error = QString("Failed to open database: %1").arg(db_manager.error());
            return false;
        }

        QStringList queries;

        queries << R"(
            CREATE TABLE IF NOT EXISTS crypto_data (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                symbol TEXT UNIQUE NOT NULL,
                base_coin TEXT,
                quote_coin TEXT
            )
        )";

        queries << "CREATE INDEX IF NOT EXISTS idx_crypto_coin ON crypto_data(base_coin)";

        if (!db_manager.execInTransaction(queries)) {
            m_last_error = QString("Failed to create database structure: %1").arg(db_manager.error());
            return false;
        }

        return true;
    }

    void DBHash::close() {
        auto& db_manager = DataBaseManager::instance();
        db_manager.close();
    }

    bool DBHash::addItem(const TradingInfo& trade_item) {
        auto& db_manager = DataBaseManager::instance();

        if (!db_manager.open(m_crypto_db)) {
            m_last_error = QString("Failed to open database: %1").arg(db_manager.error());
            return false;
        }

        db_manager.requestPrepared("INSERT INTO crypto_data (symbol, base_coin, quote_coin) VALUES (?, ?, ?)",
            {trade_item.symbol, trade_item.base_coin, trade_item.quote_coin}
        );

        return true;
    }

    bool DBHash::getAllItems(QList<Engine::TradingInfo>& data) {
        auto& db_manager = DataBaseManager::instance();

        if (!db_manager.open(m_crypto_db)) {
            m_last_error = QString("Failed to open database: %1").arg(db_manager.error());
            return false;
        }

        db_manager.request("SELECT symbol, base_coin, quote_coin FROM crypto_data", [&data](QSqlQuery& query) {
            while (query.next()) {
                TradingInfo item;
                item.symbol = query.value(0).toString();
                item.base_coin = query.value(1).toString();
                item.quote_coin = query.value(2).toString();
                data.push_back(item);
            }
        });

        return true;
    }

    bool DBHash::getItems(QList<Engine::TradingInfo>& data, const QString& category) {
        auto& db_manager = DataBaseManager::instance();

        if (!db_manager.open(m_crypto_db)) {
            m_last_error = QString("Failed to open database: %1").arg(db_manager.error());
            return false;
        }

        db_manager.requestPrepared("SELECT symbol, base_coin, quote_coin FROM crypto_data WHERE quote_coin = ?", category, [category, &data](QSqlQuery& query) {
            while (query.next()) {
               TradingInfo item;
               item.symbol = query.value(0).toString();
               item.base_coin = query.value(1).toString();
               item.quote_coin = query.value(2).toString();
               data.push_back(item);
            }
        });

        return true;
    }

    QString DBHash::error() {
        return m_last_error;
    }

}