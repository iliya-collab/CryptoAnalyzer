#pragma once

#include "Managers/JsonManager.hpp"

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
