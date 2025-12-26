#pragma once

#include "Configs/ConfigObject.hpp"

struct ParamsScannerConfig {

    QList<qint64> durations;
    qint64 allDuration;
    
    QList<QString> formatDurations;
    QList<QString> pairs;

    bool enableLogs;
    bool enableAutoDisconnect;
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
