#pragma once

#include "Configs/ConfigObject.hpp"

struct APIKey {
    QString api_key;
    QString secret_key;
};

struct ParamsPlatformConfig {
    QHash<QString, APIKey> keys;
};


class PlatformConfig : public ConfigObject<PlatformConfig, ParamsPlatformConfig> {

private:

    friend class ConfigObject<PlatformConfig, ParamsPlatformConfig>;
    PlatformConfig() = default;

public:

    QJsonObject toJson() override;

    void fromJson(const QJsonObject& obj) override;

};
