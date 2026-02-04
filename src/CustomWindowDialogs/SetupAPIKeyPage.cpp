#include "CustomWindowDialogs/SetupAPIKeyPage.hpp"

void SetupAPIKeyPage::createPage() {
    QVBoxLayout* layout = new QVBoxLayout(this);
    select_key = new QComboBox(this);
    select_key->addItems(curConfig.keys.keys());

    QHBoxLayout* row1 = new QHBoxLayout;
    QLabel* lblAPIKey = new QLabel("API Key : ", this);
    api_key = new QLabel(curConfig.keys.value(select_key->currentText()).api_key, this);
    row1->addWidget(lblAPIKey);
    row1->addWidget(api_key);

    QHBoxLayout* row2 = new QHBoxLayout;
    QLabel* lblSecretKey = new QLabel("Secret Key : ", this);
    secret_key = new QLabel(curConfig.keys.value(select_key->currentText()).secret_key, this);
    row2->addWidget(lblSecretKey);
    row2->addWidget(secret_key);

    layout->addWidget(select_key);
    layout->addLayout(row1);
    layout->addLayout(row2);
    layout->addStretch();
}

void SetupAPIKeyPage::readConfig() {
}