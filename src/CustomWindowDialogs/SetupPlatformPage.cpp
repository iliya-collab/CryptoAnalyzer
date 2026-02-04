#include "CustomWindowDialogs/SetupPlatformPage.hpp"

void SetupPlatformPage::createPage() {
    QVBoxLayout* layout = new QVBoxLayout(this);

    editCoinsPairs = new QTextEdit(this);

    /*for (auto i : curConfig.pairs)
        editCoinsPairs->append(i);*/
;
    layout->addWidget(editCoinsPairs);
}

void SetupPlatformPage::readConfig() {
    //curConfig.pairs = editCoinsPairs->toPlainText().split('\n');
}