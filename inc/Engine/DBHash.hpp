#pragma once

#include "Managers/DataBaseManager.hpp"

class DBHash {
private:
    struct ItemDB {
        QString sym;
        QString name_pair;
        QString market;
        QString full_name;
        QString description;
        QString url;
        QString path_icon;
    };

    const QString m_crypto_db = "crypto_data.db";
    
    DataBaseManager m_db;

public:

    bool dbExist();
    
    void create();

};
