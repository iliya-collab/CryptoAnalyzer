#include "CustomWindowDialogs/SetupMyWalletPage.hpp"
#include "Configs/MyWalletConfig.hpp"
#include "Configs/ScanerConfig.hpp"

QMap<QString, double> SetupMyWalletPage::parseAsset() {
    QMap<QString, double> _asset;
    QStringList lst = editAsset->toPlainText().split('\n');

    for (auto i : lst) {
        auto pairs = i.split(' ');
        _asset[pairs[0]] = pairs[1].toDouble();
    }

    return _asset;
}

QMap<QString, QString> SetupMyWalletPage::parseRules() {
    QMap<QString, QString> _rules;
    QStringList lst = editRules->toPlainText().split('\n');

    for (auto i : lst) {
        auto pairs = i.split(' ');
        _rules[pairs[0]] = pairs[1] + " " + pairs[2];
    }

    return _rules;
}

void SetupMyWalletPage::createPage() {
    auto& curWalletConfig = MyWalletConfig::instance().getConfig();
    auto curScanerConfig = ScanerConfig::instance().getConfig();

    QVBoxLayout* layout = new QVBoxLayout(this);

    editAsset = new QTextEdit(this);
    editRules = new QTextEdit(this);

    rulesScan = new QComboBox(this);

    rulesScan->addItems(curScanerConfig.formatDurations);

    for (auto [name, amount] : curWalletConfig.asset.asKeyValueRange())
        editAsset->append(QString("%1 %2").arg(name).arg(amount));

    for (auto [name, rules] : curWalletConfig.rules.asKeyValueRange())
        editRules->append(QString("%1 %2").arg(name).arg(rules));

    layout->addWidget(editAsset);
    layout->addWidget(rulesScan);
    layout->addWidget(editRules);
    layout->addStretch();
}

void SetupMyWalletPage::readConfig() {
    auto& cur = MyWalletConfig::instance().getConfig();
    cur.asset = parseAsset();
    cur.rules = parseRules();
}
