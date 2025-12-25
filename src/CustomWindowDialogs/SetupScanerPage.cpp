#include "CustomWindowDialogs/SetupScanerPage.hpp"
#include "Configs/ScanerConfig.hpp"
#include "Parser/RegularParser.hpp"

void SetupScanerPage::createPage() {
    auto& cur = ScanerConfig::instance().getConfig();

    QVBoxLayout* layout = new QVBoxLayout(this);

    checkEnableLogs = new QCheckBox(this);
    checkEnableAutoDisconnect = new QCheckBox(this);

    QHBoxLayout* row1 = new QHBoxLayout;
    row1->addWidget(checkEnableLogs);
    row1->addWidget(checkEnableAutoDisconnect);

    editFormatDuration = new QLineEdit(this);
    editCoinsPairs = new QTextEdit(this);

    checkEnableLogs->setText("Logs");
    checkEnableLogs->setChecked(cur.enableLogs);

    checkEnableAutoDisconnect->setText("Auto disconnect");
    checkEnableAutoDisconnect->setChecked(cur.enableAutoDisconnect);

    QString str = cur.formatDurations[0];
    for (int i = 1; i < cur.formatDurations.size(); i++)
        str += QString(";%1").arg(cur.formatDurations[i]);
    editFormatDuration->setText(str);

    for (auto i : cur.pairs)
        editCoinsPairs->append(i);

    layout->addLayout(row1);
    layout->addWidget(editFormatDuration);
    layout->addWidget(editCoinsPairs);
}

void SetupScanerPage::readConfig() {
    auto& cur = ScanerConfig::instance().getConfig();

    cur.enableLogs = checkEnableLogs->isChecked();
    cur.enableAutoDisconnect = checkEnableAutoDisconnect->isChecked();

    auto lst = editFormatDuration->text().split(';');
    cur.formatDurations.clear();
    for (auto i : lst) 
        cur.formatDurations.append(i);

    cur.pairs = editCoinsPairs->toPlainText().split('\n');
}
