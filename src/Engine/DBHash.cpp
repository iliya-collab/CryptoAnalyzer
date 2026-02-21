#include "Engine/DBHash.hpp"

bool Engine::DBHash::dbExist() {
    auto& db_manager = DataBaseManager::instance();
    return db_manager.existDBFile(m_crypto_db);
}

void Engine::DBHash::create() {
    auto& db_manager = DataBaseManager::instance();
    
    if (!db_manager.open(m_crypto_db)) {
        qWarning() << "Failed to open database:" << db_manager.error();
        return;
    }

    QStringList queries;
    
    queries << R"(
        CREATE TABLE IF NOT EXISTS crypto_data (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            symbol TEXT UNIQUE NOT NULL,
            base_coin TEXT,
            quote_coin TEXT,
            status TEXT,
            category TEXT
        )
    )";
    
    queries << "CREATE INDEX IF NOT EXISTS idx_crypto_coin ON crypto_data(base_coin)";

    if (!db_manager.execInTransaction(queries))
        qWarning() << "Failed to create database structure:" << db_manager.error();
    else
        qInfo() << "Database structure created successfully";
}

void Engine::DBHash::close() {
    auto& db_manager = DataBaseManager::instance();
    db_manager.close();
}

void Engine::DBHash::open() {
    auto& db_manager = DataBaseManager::instance();
    if (!db_manager.open(m_crypto_db))
        qWarning() << "Failed to open database:" << db_manager.error();
}


void Engine::DBHash::addItem(const TradingInfo& trade_item) {
    auto& db_manager = DataBaseManager::instance();
    
    if (!db_manager.open(m_crypto_db)) {
        qWarning() << "Failed to open database for crypto item:" << db_manager.error();
        return;
    }

    if (!db_manager.requestPrepared("INSERT OR REPLACE INTO crypto_data (symbol, base_coin, quote_coin, status, category) VALUES (?, ?, ?, ?, ?)", 
        {trade_item.symbol, trade_item.base_coin, trade_item.quote_coin, trade_item.status, trade_item.category}))
        qWarning() << "Failed to add trading record:" << db_manager.error();
}