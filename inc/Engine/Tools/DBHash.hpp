#pragma once

#include "Engine/Managers/DataBaseManager.hpp"
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
        bool getAllItems(QHash<QString, QList<Engine::TradingInfo>>& data);
    };
}

