#pragma once

#include "Configs/ConfigObject.hpp"

struct ParamsScanerConfig {

    QList<qint64> durations;
    qint64 allDuration;
    
    QList<QString> formatDurations;
    QList<QString> pairs;

    bool enableLogs;
    bool enableAutoDisconnect;
};


class ScanerConfig : public ConfigObject<ScanerConfig, ParamsScanerConfig> {

private:

    friend class ConfigObject<ScanerConfig, ParamsScanerConfig>;
    ScanerConfig() = default;

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
