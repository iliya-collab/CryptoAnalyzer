#ifndef DIALOG_SETUP_MENU_HPP
#define DIALOG_SETUP_MENU_HPP

#include "CustomWindowDialogs/IDialog.hpp"
#include "SetupPlatformPage.hpp"
#include "SetupAPIKeyPage.hpp"

#include <QListWidget>
#include <QStackedWidget>
#include <QPushButton>

class DialogSetupMenu : public IDialog {

private:

    QListWidget* contentsWidget;
    QStackedWidget* pagesWidget;

    QPushButton* applyButton;

    SetupPlatformPage* setup_platform_page;
    SetupAPIKeyPage* setup_api_key_page;

    QHBoxLayout* mainLayout;

    void setupUI() override;
    void connectionSignals();

private slots:

    void onClickedButtonApply();
    void changePage(QListWidgetItem *current, QListWidgetItem *previous);

public:

    DialogSetupMenu(QWidget* parent = nullptr);

};

#endif
