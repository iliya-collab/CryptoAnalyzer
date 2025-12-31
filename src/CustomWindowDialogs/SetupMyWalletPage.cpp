#include "CustomWindowDialogs/SetupMyWalletPage.hpp"
#include "Configs/MyWalletConfig.hpp"

QMap<QString, double> SetupMyWalletPage::parseAsset() {
    QMap<QString, double> _asset;
    QStringList lst = editAsset->toPlainText().split('\n');

    for (auto i : lst) {
        auto pairs = i.split(' ');
        _asset[pairs[0]] = pairs[1].toDouble();
    }

    return _asset;
}

void SetupMyWalletPage::createPage() {
    auto& curWalletConfig = MyWalletConfig::instance().getConfig();

    QVBoxLayout* layout = new QVBoxLayout(this);

    editAsset = new QTextEdit(this);


    for (auto [name, amount] : curWalletConfig.asset.asKeyValueRange())
        editAsset->append(QString("%1 %2").arg(name).arg(amount));

    layout->addWidget(editAsset);
    layout->addStretch();
}

void SetupMyWalletPage::readConfig() {
    auto& cur = MyWalletConfig::instance().getConfig();
    cur.asset = parseAsset();
}
