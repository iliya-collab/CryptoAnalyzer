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
            sym TEXT UNIQUE NOT NULL,
            full_name TEXT,
            description TEXT,
            url_icon TEXT
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


void Engine::DBHash::addItem(const ItemCrypto& item) {
    auto& db_manager = DataBaseManager::instance();

    if (!db_manager.open(m_crypto_db)) {
        qWarning() << "Failed to open database for crypto item:" << db_manager.error();
        return;
    }

    if (!db_manager.requestPrepared("INSERT OR REPLACE INTO crypto_data (sym, full_name, description, url_icon) VALUES (?, ?, ?, ?)", {item.sym, item.full_name, item.description, item.url_icon})) 
        qWarning() << "Failed to add record to database:" << db_manager.error();

}

void Engine::DBHash::addItem(const ItemTrading& item) {
    auto& db_manager = DataBaseManager::instance();
    
    if (!db_manager.open(m_crypto_db)) {
        qWarning() << "Failed to open database for trading item:" << db_manager.error();
        return;
    }

    int crypto_id = -1;
    QString sym = item.sym;

    db_manager.requestPrepared("SELECT id FROM crypto_data WHERE sym = ?", {item.sym}, [&crypto_id, sym] (QSqlQuery& query) {
        if (!query.next()) {
            qWarning() << "Crypto not found for sym:" << sym;
            return;
        }
        
        crypto_id = query.value(0).toInt();
    });
    
    // Теперь вставляем в trading_data со всеми полями
    if (!db_manager.requestPrepared(
        "INSERT OR REPLACE INTO trading_data (crypto_id, sym, name_pair, stock_market, market) VALUES (?, ?, ?, ?, ?)",
        {crypto_id, item.sym, item.name_pair, item.stock_market, item.market}
    )) {
        qWarning() << "Failed to add trading record:" << db_manager.error();
    }
}

Engine::DBHash::ItemTrading Engine::convertTo(const TradingInfo& item) {
    DBHash::ItemTrading itemTrading = {
        .sym = item.sym,
        .name_pair = item.namePair,
        .stock_market = item.stockMarket,
        .market = item.market
    };
    return itemTrading;
}

Engine::DBHash::ItemCrypto Engine::convertTo(const InfoAboutCoin& item) {
    DBHash::ItemCrypto itemCrypto = {
        .sym = item.sym,
        .full_name = item.fullName,
        .description = item.description,
        .url_icon = item.coinIcon.url
    };
    return itemCrypto;
}

Engine::TradingInfo Engine::convertFrom(const DBHash::ItemTrading& item) {
    TradingInfo tradingInfo = {

    };
    return tradingInfo;
}

Engine::InfoAboutCoin Engine::convertFrom(const DBHash::ItemCrypto& item) {
    InfoAboutCoin infoAboutCoin = {

    };
    return infoAboutCoin;
}