#include "Configs/ScanerConfig.hpp"
#include "Parser/RegularParser.hpp"

QJsonObject ScanerConfig::toJson() {
    QJsonObject scaner;
    scaner["Logs"] = _config.enableLogs;
    scaner["AutoDisconnect"] = _config.enableAutoDisconnect;

    QJsonArray arrDurations;
    for (auto i : _config.formatDurations)
        arrDurations.append(i);
    scaner["Duration"] = arrDurations;

    QJsonArray arrPairs;
    for (auto i : _config.pairs)
        arrPairs.append(i);
    scaner["Pairs"] = arrPairs;

    return scaner;
}

void ScanerConfig::setDefaultConfig() {
    _config.enableLogs = false;
    _config.enableAutoDisconnect = true;

    _config.allDuration = 0;

    _config.formatDurations = {};
    _config.durations = {};
    _config.pairs = {};
}

void ScanerConfig::fromJson(const QJsonObject& obj) {
    _config.enableLogs = obj.value("Logs").toBool();
    _config.enableAutoDisconnect = obj.value("AutoDisconnect").toBool();

    QJsonArray arrDurations = obj.value("Duration").toArray();
    _config.formatDurations.clear();
    for (int i = 0; i < arrDurations.size(); i++)
        _config.formatDurations.append(arrDurations[i].toString());

    RegularParser reg;
    QStringList lst;

    _config.durations.clear();
    _config.allDuration = 0;
    for (auto i : _config.formatDurations) 
        if (reg.insertMultValues(lst, i)) {
            for (auto j : lst) {
                _config.allDuration += j.toLongLong();
                _config.durations.append(j.toLongLong());
            }
            lst.clear();
        }
        else {
            _config.allDuration += i.toLongLong();
            _config.durations.append(i.toLongLong());
        }

    
    QJsonArray arrPairs = obj.value("Pairs").toArray();
    _config.pairs.clear();
    for (int i = 0; i < arrPairs.size(); i++)
        _config.pairs.append(arrPairs[i].toString());
}
