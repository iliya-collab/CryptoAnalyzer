#pragma once

#include "CustomWindowDialogs/IDialog.hpp"
#include "SetupPlatformPage.hpp"
#include "SetupAPIPage.hpp"

#include <QListWidget>
#include <QStackedWidget>
#include <QPushButton>

class DialogSetupMenu : public IDialog {

private:

    QListWidget* contentsWidget;
    QStackedWidget* pagesWidget;

    QPushButton* applyButton;

    SetupPlatformPage* setup_platform_page;
    SetupAPIPage* setup_api_page;

    QHBoxLayout* mainLayout;

    void setupUI() override;
    void connectionSignals();

private slots:

    void onClickedButtonApply();
    void changePage(QListWidgetItem *current, QListWidgetItem *previous);

public:

    DialogSetupMenu(QWidget* parent = nullptr);

};
