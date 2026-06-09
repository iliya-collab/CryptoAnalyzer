#include "Engine/Tools/DBHash.hpp"
#include "Engine/Tools/DataBaseManager.hpp"

namespace Engine {

    bool DBHash::dbExist(const QString& name) {
        auto& db_manager = DataBaseManager::instance();
        return db_manager.existDBFile(name);
    }

    bool DBHash::createCryptoDB() {
        auto& db_manager = DataBaseManager::instance();

        if (!db_manager.open(m_dbCrypto)) {
            m_lastError = QString("Failed to open database: %1").arg(db_manager.error());
            return false;
        }

        QString query = R"(
            CREATE TABLE IF NOT EXISTS crypto_data (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                symbol TEXT UNIQUE NOT NULL,
                base_coin TEXT,
                quote_coin TEXT
            )
        )";

        if (!db_manager.request(m_dbCrypto, query)) {
            m_lastError = db_manager.error();
            return false;
        }

        return true;
    }

    void DBHash::close(const QString& dbPath) {
        auto& db_manager = DataBaseManager::instance();
        db_manager.close(dbPath);
    }

    bool DBHash::addItems(const QList<TradingInfo>& items) {
        if (items.isEmpty())
            return true;

        auto& db_manager = DataBaseManager::instance();

        if (!db_manager.open(m_dbCrypto)) {
            m_lastError = db_manager.error();
            return false;
        }

        if (!db_manager.beginTransaction(m_dbCrypto))
            return false;

        for (const auto& item : items)
            if (!db_manager.requestPrepared(m_dbCrypto, "INSERT INTO crypto_data (symbol, base_coin, quote_coin) VALUES (?, ?, ?)", { item.symbol, item.base_coin, item.quote_coin })) {
                db_manager.rollbackTransaction(m_dbCrypto);
                return false;
            }

        return db_manager.commitTransaction(m_dbCrypto);
    }

    bool DBHash::getAllItems(QList<Engine::TradingInfo>& data) {
        auto& db_manager = DataBaseManager::instance();

        if (!db_manager.open(m_dbCrypto)) {
            m_lastError = QString("Failed to open database: %1").arg(db_manager.error());
            return false;
        }

        db_manager.request(m_dbCrypto, "SELECT symbol, base_coin, quote_coin FROM crypto_data", [&data](QSqlQuery& query) {
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

        if (!db_manager.open(m_dbCrypto)) {
            m_lastError = QString("Failed to open database: %1").arg(db_manager.error());
            return false;
        }

        db_manager.requestPrepared(m_dbCrypto, "SELECT symbol, base_coin, quote_coin FROM crypto_data WHERE quote_coin = ?", category, [category, &data](QSqlQuery& query) {
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
        return m_lastError;
    }

}