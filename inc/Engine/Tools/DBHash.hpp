#pragma once

#include "Engine/StdTypes.hpp"

namespace Engine {

    class DBHash {
    private:
    
        const QString m_crypto_db = "crypto_data.db";
        QString m_last_error;
    
    public:
    
        bool dbExist();    
        bool create();
        void close();
        QString error();
        
        bool addItem(const TradingInfo& trade_item);
        bool getAllItems(const QString& category, QList<Engine::TradingInfo>& data);
    };
}

