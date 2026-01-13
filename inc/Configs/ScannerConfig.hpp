#pragma once

#include "Configs/ConfigObject.hpp"

struct ParamsScannerConfig {
    QList<QString> pairs;
};


class ScannerConfig : public ConfigObject<ScannerConfig, ParamsScannerConfig> {

private:

    friend class ConfigObject<ScannerConfig, ParamsScannerConfig>;
    ScannerConfig() = default;

public:

    QJsonObject toJson() override;

    void setDefaultConfig() override;

    void fromJson(const QJsonObject& obj) override;

    QString getName() override {
        return "Scaner";
    }

    bool isJsonObjectValid(const QJsonObject& obj) override {
        return true;
    }

};
