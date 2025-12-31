#ifndef SETUP_MY_WALLET_PAGE
#define SETUP_MY_WALLET_PAGE

#include "SetupPage.hpp"

#include <QTextEdit>

class SetupMyWalletPage : public SetupPage {
private:

    QTextEdit* editAsset;

    QMap<QString, double> parseAsset();

public:

    SetupMyWalletPage(QWidget* parent = nullptr) : SetupPage(parent) {}

    void createPage() override;

    void readConfig() override;


};

#endif // SETUP_MY_WALLET_PAGE
