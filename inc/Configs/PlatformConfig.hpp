#pragma once

#include "Configs/ConfigObject.hpp"

struct ParamsPlatformConfig {
    QList<QString> pairs;
};


class PlatformConfig : public ConfigObject<PlatformConfig, ParamsPlatformConfig> {

private:

    friend class ConfigObject<PlatformConfig, ParamsPlatformConfig>;
    PlatformConfig() = default;

public:

    QJsonObject toJson() override;

    void setDefaultConfig() override;

    void fromJson(const QJsonObject& obj) override;

    std::expected<QJsonObject, QString> isJsonObjectValid(const QJsonObject& obj) override;

};
