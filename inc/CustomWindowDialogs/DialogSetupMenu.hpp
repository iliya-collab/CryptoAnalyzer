#ifndef DIALOG_SETUP_MENU_HPP
#define DIALOG_SETUP_MENU_HPP

#include "CustomQDialog.hpp"
#include "SetupScanerPage.hpp"
#include "SetupMyWalletPage.hpp"

#include <QListWidget>
#include <QStackedWidget>
#include <QPushButton>

class DialogSetupMenu : public CustomQDialog {

private:

    QListWidget* contentsWidget;
    QStackedWidget* pagesWidget;

    QPushButton* applyButton;

    SetupScanerPage* setup_scaner_page;
    SetupMyWalletPage* setup_my_wallet_page;

    void setupUI() override;
    void connectionSignals() override;

private slots:
    void onClickedButtonApply();
    void changePage(QListWidgetItem *current, QListWidgetItem *previous);

public:

    DialogSetupMenu(QWidget* parent = nullptr);


};

#endif
