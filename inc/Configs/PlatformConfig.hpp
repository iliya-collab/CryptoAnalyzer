#pragma once

#include "Configs/ConfigObject.hpp"
#include "Engine/StdTypes.hpp"

struct ParamsPlatformConfig {
    QHash<QString, Engine::API> m_api;
};


class PlatformConfig : public ConfigObject<PlatformConfig, ParamsPlatformConfig> {

private:

    friend class ConfigObject<PlatformConfig, ParamsPlatformConfig>;
    PlatformConfig() = default;

public:

    QJsonObject toJson() override;

    void fromJson(const QJsonObject& obj) override;

};
