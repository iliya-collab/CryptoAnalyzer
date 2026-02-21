#pragma once

#include "Managers/DataBaseManager.hpp"
#include "Engine/StdTypes.hpp"

/*
CREATE TABLE IF NOT EXISTS crypto_data (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    sym TEXT UNIQUE NOT NULL,
    full_name TEXT,
    description TEXT,
    url TEXT,
    path_icon TEXT
);

CREATE TABLE IF NOT EXISTS trading_data (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    crypto_id INTEGER NOT NULL,
    sym TEXT NOT NULL,  -- денормализовано для быстрого доступа
    name_pair TEXT,
    market TEXT,
    FOREIGN KEY (crypto_id) REFERENCES crypto_data(id) ON DELETE CASCADE,
    FOREIGN KEY (sym) REFERENCES crypto_data(sym) ON UPDATE CASCADE
);

-- Создание индексов
CREATE INDEX idx_trading_crypto_id ON trading_data(crypto_id);
CREATE INDEX idx_trading_sym ON trading_data(sym);
CREATE INDEX idx_crypto_sym ON crypto_data(sym);

-- Пример запроса с JOIN
SELECT 
    c.sym,
    c.full_name,
    t.name_pair,
    t.market
FROM crypto_data c
LEFT JOIN trading_data t ON c.id = t.crypto_id
WHERE c.sym = 'BTC';

-- Очистка таблиц (если нужно перезаполнить)
DELETE FROM trading_data;
DELETE FROM crypto_data;

-- Вставка данных в crypto_data
INSERT INTO crypto_data (sym, full_name, description, url, path_icon) VALUES
-- Топ криптовалюты
('BTC', 'Bitcoin', 'Первая и самая популярная криптовалюта. Создана в 2009 году Сатоши Накамото.', 'https://bitcoin.org', '/icons/btc.png'),

INSERT INTO trading_data (crypto_id, sym, name_pair, market) VALUES
-- Bitcoin пары
((SELECT id FROM crypto_data WHERE sym = 'BTC'), 'BTC', 'BTC/USDT', 'Binance'),
((SELECT id FROM crypto_data WHERE sym = 'BTC'), 'BTC', 'BTC/USD', 'Coinbase'),
((SELECT id FROM crypto_data WHERE sym = 'BTC'), 'BTC', 'BTC/EUR', 'Kraken'),

-- Проверка вставленных данных
SELECT 'crypto_data count: ' || COUNT(*) FROM crypto_data;
SELECT 'trading_data count: ' || COUNT(*) FROM trading_data;

-- Пример запроса для проверки связей
SELECT 
    c.sym,
    c.full_name,
    t.name_pair,
    t.market
FROM crypto_data c
JOIN trading_data t ON c.id = t.crypto_id
WHERE c.sym IN ('BTC', 'ETH')
ORDER BY c.sym, t.market;
*/

namespace Engine {

    class DBHash {
    private:
    
    const QString m_crypto_db = "crypto_data.db";
    
    public:
    
        bool dbExist();    
        void create();
        void close();
        void open();
        
        void addItem(const TradingInfo& trade_item);
        QList<TradingInfo> getAllItems();
    };
}

