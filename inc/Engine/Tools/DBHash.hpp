#pragma once

#include "Engine/Tools/StdTypes.hpp"

namespace Engine {

    class DBHash {
    private:
    
        QString m_lastError;
    
    public:

        const QString m_dbCrypto = "crypto_data.db";
        const QString m_dbHistory = "history_candles.db";

        bool dbExist(const QString& name);

        bool createCryptoDB();

        void close(const QString& dbPath);
        QString error();
        
        bool addItems(const QList<TradingInfo>& items);
        bool getAllItems(QList<Engine::TradingInfo>& data);
        bool getItems(QList<Engine::TradingInfo>& data, const QString& category);
    };
}

