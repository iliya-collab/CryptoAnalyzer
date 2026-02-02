#include "Managers/Settings.hpp"

#include "Configs/PlatformConfig.hpp"

QJsonObject Settings::root = {};
QString Settings::l_error = "";
const char* Settings::configFile = "platform.json";

void Settings::getDefaultConfig() {
    PlatformConfig::instance().setDefaultConfig();
}

bool Settings::readAllConfig() {
    QJsonDocument doc;
    auto exp = JsonManager::readDocument(configFile);
    if (exp.has_value()) {
        doc = exp.value();
        exp = JsonManager::isDocumentValid(doc);
        if (exp.has_value())
            parseJsonDocument(doc);
        else {
            l_error = exp.error();
            return false;
        }
    }
    else {
        l_error = exp.error();
        return false;
    }
    return true;
}

void Settings::parseJsonDocument(const QJsonDocument& doc) {
    root = doc.object();
    PlatformConfig::instance().fromJson(root.value("Platform").toObject());
}


void Settings::writeAllConfig() {
    root["Platform"] = PlatformConfig::instance().toJson();
    QJsonDocument doc(root);
    JsonManager::setDocument(doc);
    JsonManager::writeDocument(configFile);
}
