#include "CustomWindowDialogs/SetupScanerPage.hpp"
#include "Parser/RegularParser.hpp"

void SetupScanerPage::createPage() {
    QVBoxLayout* layout = new QVBoxLayout(this);

    checkEnableLogs = new QCheckBox(this);
    checkEnableAutoDisconnect = new QCheckBox(this);
    comboDurations = new EditableComboBox(this);
    editRules = new QTextEdit(this);
    editCoinsPairs = new QTextEdit(this);

    connect(comboDurations, &EditableComboBox::itemSelected, this, &SetupScanerPage::onComboIndexChanged);
    connect(comboDurations, &EditableComboBox::returnPressed, this, &SetupScanerPage::onEditingFinished);

    QHBoxLayout* row1 = new QHBoxLayout;
    row1->addWidget(checkEnableLogs);
    row1->addWidget(checkEnableAutoDisconnect);

    QHBoxLayout* row2 = new QHBoxLayout;
    row2->addWidget(comboDurations);

    QHBoxLayout* row3 = new QHBoxLayout;
    row3->addWidget(editCoinsPairs);
    row3->addWidget(editRules);

    checkEnableLogs->setText("Logs");
    checkEnableLogs->setChecked(curConfig.enableLogs);
    checkEnableAutoDisconnect->setText("Auto disconnect");
    checkEnableAutoDisconnect->setChecked(curConfig.enableAutoDisconnect);
    comboDurations->setInitItems(curConfig.formatDurations);
    comboDurations->setCurrentIndex(0);
    for (auto i : curConfig.pairs)
        editCoinsPairs->append(i);

    layout->addLayout(row1);
    layout->addLayout(row2);
    layout->addLayout(row3);
}

void SetupScanerPage::readConfig() {
    curConfig.enableLogs = checkEnableLogs->isChecked();
    curConfig.enableAutoDisconnect = checkEnableAutoDisconnect->isChecked();
    curConfig.pairs = editCoinsPairs->toPlainText().split('\n');
    curConfig.rules[comboDurations->currentIndex()] = parseRules();

    QStringList texts;
    for (int i = 0; i < comboDurations->count(); ++i) {
        texts.append(comboDurations->itemText(i));
    }
    curConfig.formatDurations = texts;
}

QMap<QString, QString> SetupScanerPage::parseRules() {
    QMap<QString, QString> _rules;
    QStringList lst = editRules->toPlainText().split('\n');

    for (auto i : lst) {
        auto pairs = i.split(' ');
        _rules[pairs[0]] = pairs[1] + " " + pairs[2];
    }

    return _rules;
}

void SetupScanerPage::onComboIndexChanged(const QString& text, int index) {
    editRules->clear();
    for (auto [name, rules] : curConfig.rules[index].asKeyValueRange())
        editRules->append(QString("%1 %2").arg(name).arg(rules));
}

void SetupScanerPage::onEditingFinished(const QString& text) {
    comboDurations->addItem(text);
    curConfig.formatDurations.append(text);
    curConfig.rules.append(QMap<QString, QString>());
}