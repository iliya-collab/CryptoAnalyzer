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
            category TEXT
        )
    )";
    
    queries << "CREATE INDEX IF NOT EXISTS idx_crypto_coin ON crypto_data(base_coin)";

    if (!db_manager.execInTransaction(queries))
        qWarning() << "Failed to create database structure:" << db_manager.error();
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
        qWarning() << "Failed to open database:" << db_manager.error();
        return;
    }
    
    db_manager.requestPrepared("INSERT INTO crypto_data (symbol, base_coin, quote_coin, category) VALUES (?, ?, ?, ?)",
        {trade_item.symbol, trade_item.base_coin, trade_item.quote_coin, trade_item.category}
    );
}

QList<Engine::TradingInfo> Engine::DBHash::getAllItems() {
    auto& db_manager = DataBaseManager::instance();
    QList<TradingInfo> items;
    
    if (!db_manager.open(m_crypto_db)) {
        qWarning() << "Failed to open database:" << db_manager.error();
        return items;
    }
    
    db_manager.request("SELECT symbol, base_coin, quote_coin, category FROM crypto_data", [&items](QSqlQuery& query) {
        while (query.next()) {
            TradingInfo item;
            item.symbol = query.value(0).toString();
            item.base_coin = query.value(1).toString();
            item.quote_coin = query.value(2).toString();
            item.category = query.value(3).toString();
            items.push_back(item);
        }
    });
    
    return items;
}