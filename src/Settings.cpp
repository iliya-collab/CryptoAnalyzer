#include "Settings.hpp"

QJsonObject Settings::root = {};
QString Settings::l_error = "";
const char* Settings::configFile = "config.json";

void Settings::getDefaultConfig() {

    ScannerConfig::instance().setDefaultConfig();
    MyWalletConfig::instance().setDefaultConfig();
    
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

    ScannerConfig::instance().fromJson(root.value(ScannerConfig::instance().getName()).toObject());
    MyWalletConfig::instance().fromJson(root.value(MyWalletConfig::instance().getName()).toObject());

}


void Settings::writeAllConfig() {

    root[ScannerConfig::instance().getName()] = ScannerConfig::instance().toJson();
    root[MyWalletConfig::instance().getName()] = MyWalletConfig::instance().toJson();
    
    QJsonDocument doc(root);
    JsonManager::setDocument(doc);
    JsonManager::writeDocument(configFile);

}