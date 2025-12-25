#ifndef _SETTINGS_
#define _SETTINGS_

#include "JsonManager.hpp"

#include "Configs/ScanerConfig.hpp"
#include "Configs/MyWalletConfig.hpp"

#ifndef OPERATION_STATUS
#define OPERATION_STATUS

#define OK  "\033[32m✔\033[0m"
#define ERR "\033[31m✗\033[0m"
#define WAR "\033[33m⚠\033[0m"

#endif

/*
Settings - объект, который считывает все конфиги и настраевает их (вызывается один раз в файле main.cpp)
*/

class Settings {

private:

    static QJsonObject root;

    static void parseJsonDocument(const QJsonDocument& doc);

    Settings() = default;
    
    static const char* configFile;

    static QString l_error;

public:


    static void getDefaultConfig();

    static bool readAllConfig();
    static void writeAllConfig();

    static QString getLastError() {
        return l_error;
    }

};


#endif