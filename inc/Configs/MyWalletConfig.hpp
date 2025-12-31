#ifndef MY_WALLET_CONFIG
#define MY_WALLET_CONFIG

#include "Configs/ConfigObject.hpp"
#include <QMap>

struct ParamsMyWalletConfig {
    QMap<QString, double> asset;
};


class MyWalletConfig : public ConfigObject<MyWalletConfig, ParamsMyWalletConfig> {

private:

    friend class ConfigObject<MyWalletConfig, ParamsMyWalletConfig>;
    MyWalletConfig() = default;

public:

    QJsonObject toJson() override;

    void setDefaultConfig() override {
        _config.asset = {};
    }

    void fromJson(const QJsonObject& obj) override;

    QString getName() override {
        return "MyWallet";
    }

    bool isJsonObjectValid(const QJsonObject& obj) override {
        return true;
    }

};

#endif // MY_WALLET_CONFIG
