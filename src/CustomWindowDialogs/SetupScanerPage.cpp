#include "CustomWindowDialogs/SetupScanerPage.hpp"

void SetupScanerPage::createPage() {
    QVBoxLayout* layout = new QVBoxLayout(this);

    editCoinsPairs = new QTextEdit(this);

    for (auto i : curConfig.pairs)
        editCoinsPairs->append(i);
;
    layout->addWidget(editCoinsPairs);
}

void SetupScanerPage::readConfig() {
    curConfig.pairs = editCoinsPairs->toPlainText().split('\n');
}