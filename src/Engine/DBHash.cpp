#include "Engine/DBHash.hpp"

bool DBHash::dbExist() {
    auto& db_manager = DataBaseManager::instance();
    return db_manager.existDBFile(m_crypto_db);
}

void DBHash::create() {
    /*auto& db_manager = DataBaseManager::instance();
    
    if (!db_manager.open(m_crypto_db)) {
        qWarning() << "Failed to open database:" << db_manager.error();
        return;
    }

    if (!db_manager.request("BEGIN TRANSACTION")) {
        qWarning() << "Failed to begin transaction:" << db_manager.error();
        return;
    }

    bool success = true;

    success &= db_manager.request(R"(
        CREATE TABLE IF NOT EXISTS crypto_data (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            sym TEXT UNIQUE NOT NULL,
            full_name TEXT,
            description TEXT,
            url TEXT,
            path_icon TEXT
        )
    )");

    success &= db_manager.request(R"(
        CREATE TABLE IF NOT EXISTS trading_data (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            crypto_id INTEGER NOT NULL,
            sym TEXT NOT NULL,
            name_pair TEXT,
            market TEXT,
            FOREIGN KEY (crypto_id) REFERENCES crypto_data(id) ON DELETE CASCADE,
            FOREIGN KEY (sym) REFERENCES crypto_data(sym) ON UPDATE CASCADE
        )
    )");

    success &= db_manager.request("CREATE INDEX IF NOT EXISTS idx_trading_crypto_id ON trading_data(crypto_id)");

    success &= db_manager.request("CREATE INDEX IF NOT EXISTS idx_trading_sym ON trading_data(sym)");

    success &= db_manager.request("CREATE INDEX IF NOT EXISTS idx_crypto_sym ON crypto_data(sym)");

    if (success && !db_manager.request("COMMIT"))
        qWarning() << "Failed to commit transaction:" << db_manager.error();
    else {
        db_manager.request("ROLLBACK");
        qWarning() << "Failed to create database structure:" << db_manager.error();
    }*/
    auto& db_manager = DataBaseManager::instance();
    
    if (!db_manager.open(m_crypto_db)) {
        qWarning() << "Failed to open database:" << db_manager.error();
        return;
    }

    QStringList queries;
    
    queries << R"(
        CREATE TABLE IF NOT EXISTS crypto_data (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            sym TEXT UNIQUE NOT NULL,
            full_name TEXT,
            description TEXT,
            url TEXT,
            path_icon TEXT
        )
    )";
    
    queries << R"(
        CREATE TABLE IF NOT EXISTS trading_data (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            crypto_id INTEGER NOT NULL,
            sym TEXT NOT NULL,
            name_pair TEXT,
            stock_market TEXT,
            market TEXT,
            FOREIGN KEY (crypto_id) REFERENCES crypto_data(id) ON DELETE CASCADE,
            FOREIGN KEY (sym) REFERENCES crypto_data(sym) ON UPDATE CASCADE
        )
    )";
    
    queries << "CREATE INDEX IF NOT EXISTS idx_trading_crypto_id ON trading_data(crypto_id)";
    queries << "CREATE INDEX IF NOT EXISTS idx_trading_sym ON trading_data(sym)";
    queries << "CREATE INDEX IF NOT EXISTS idx_crypto_sym ON crypto_data(sym)";

    if (!db_manager.executeInTransaction(queries))
        qWarning() << "Failed to create database structure:" << db_manager.error();
    else
        qInfo() << "Database structure created successfully";
}