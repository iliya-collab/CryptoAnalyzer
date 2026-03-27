#include "Engine/Tools/DBHash.hpp"

bool Engine::DBHash::dbExist() {
    auto& db_manager = DataBaseManager::instance();
    return db_manager.existDBFile(m_crypto_db);
}

bool Engine::DBHash::create() {
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
            quote_coin TEXT,
            category TEXT
        )
    )";
    
    queries << "CREATE INDEX IF NOT EXISTS idx_crypto_coin ON crypto_data(base_coin)";

    if (!db_manager.execInTransaction(queries)) {
        m_last_error = QString("Failed to create database structure: %1").arg(db_manager.error());
        return false;
    }

    return true;
}

void Engine::DBHash::close() {
    auto& db_manager = DataBaseManager::instance();
    db_manager.close();
}


bool Engine::DBHash::addItem(const TradingInfo& trade_item) {
    auto& db_manager = DataBaseManager::instance();
    
    if (!db_manager.open(m_crypto_db)) {
        m_last_error = QString("Failed to open database: %1").arg(db_manager.error());
        return false;
    }
    
    db_manager.requestPrepared("INSERT INTO crypto_data (symbol, base_coin, quote_coin, category) VALUES (?, ?, ?, ?)",
        {trade_item.symbol, trade_item.base_coin, trade_item.quote_coin, trade_item.category}
    );

    return true;
}

bool Engine::DBHash::getAllItems(QHash<QString, QList<Engine::TradingInfo>>& data) {
    auto& db_manager = DataBaseManager::instance();
    
    if (!db_manager.open(m_crypto_db)) {
        m_last_error = QString("Failed to open database: %1").arg(db_manager.error());
        return false;
    }
    
    QStringList requestedCategory = { "spot", "linear", "inverse", "option" };

    for (const auto& next_category : requestedCategory)
        db_manager.requestPrepared("SELECT symbol, base_coin, quote_coin, category FROM crypto_data WHERE category = ?", next_category, [next_category, &data](QSqlQuery& query) {
            QList<TradingInfo> items;
            while (query.next()) {
                TradingInfo item;
                item.symbol = query.value(0).toString();
                item.base_coin = query.value(1).toString();
                item.quote_coin = query.value(2).toString();
                item.category = query.value(3).toString();
                items.push_back(item);
            }
            data.insert(next_category, items);
        });

    return true;
}

QString Engine::DBHash::error() {
    return m_last_error;
}