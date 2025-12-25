#ifndef SETUP_MY_WALLET_PAGE
#define SETUP_MY_WALLET_PAGE

#include "SetupPage.hpp"

#include <QTextEdit>
#include <QComboBox>

class SetupMyWalletPage : public SetupPage {
private:

    QTextEdit* editAsset;
    QComboBox* rulesScan;
    QTextEdit* editRules;

    QMap<QString, double> parseAsset();
    QMap<QString, QString> parseRules();

public:

    SetupMyWalletPage(QWidget* parent = nullptr) : SetupPage(parent) {}

    void createPage() override;

    void readConfig() override;


};

#endif // SETUP_MY_WALLET_PAGE
