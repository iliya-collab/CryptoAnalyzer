#include "Configs/ScannerConfig.hpp"
#include "Parser/RegularParser.hpp"

QJsonObject ScannerConfig::toJson() {
    QJsonObject scanner;
    scanner["Logs"] = _config.enableLogs;
    scanner["AutoDisconnect"] = _config.enableAutoDisconnect;

    QJsonArray arrDurations;
    for (auto i : _config.formatDurations)
        arrDurations.append(i);
    scanner["Duration"] = arrDurations;

    QJsonArray arrPairs;
    for (auto i : _config.pairs)
        arrPairs.append(i);
    scanner["Pairs"] = arrPairs;

    QJsonArray rulesArray;
    for (const auto& ruleObj : _config.rules) {
        QJsonObject rule;
        for (auto [name, amount] : ruleObj.asKeyValueRange())
            rule[name] = amount;
        rulesArray.append(rule);
    }
    scanner["Rules"] = rulesArray;

    return scanner;
}

void ScannerConfig::setDefaultConfig() {
    _config.enableLogs = false;
    _config.enableAutoDisconnect = true;
    _config.formatDurations = { "10000" };
    _config.pairs = { "BTCUSDT" };

    _config.rules.clear();
    QMap<QString, QString> rule;
    rule["BTC"] = "sell 10%";
    _config.rules.append(rule);

    
    _config.allDuration = 0;
    _config.durations = {};
}

void ScannerConfig::fromJson(const QJsonObject& obj) {
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

    QJsonArray rules = obj.value("Rules").toArray();
    _config.rules.clear();
    for (int i = 0; i < rules.size(); i++) {
        QJsonObject ruleObj = rules[i].toObject();
        QMap<QString, QString> rule;
        for (auto it = ruleObj.begin(); it != ruleObj.end(); ++it)
            rule[it.key()] = it.value().toString();
        _config.rules.append(rule);
    }
}
