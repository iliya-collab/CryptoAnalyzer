#include "Engine/DBHash.hpp"

bool DBHash::dbExist() {
    DataBaseManager db_manager;
    return db_manager.existDBFile(m_crypto_db);
}

void DBHash::create() {
    DataBaseManager db_manager;
    
    db_manager.open(m_crypto_db);

    db_manager.request(R"(
        CREATE TABLE IF NOT EXISTS items (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            sym TEXT NOT NULL UNIQUE,
            name_pair TEXT NOT NULL,
            market TEXT,
            full_name TEXT,
            description TEXT,
            url TEXT,
            path_icon TEXT,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            updated_at DATETIME DEFAULT CURRENT_TIMESTAMP
        )
    )");

    db_manager.request("CREATE INDEX IF NOT EXISTS idx_sym ON items(sym)");
    db_manager.request("CREATE INDEX IF NOT EXISTS idx_market ON items(market)");

    db_manager.close();
}