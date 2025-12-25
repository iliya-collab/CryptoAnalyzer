#pragma once

#include "SetupPage.hpp"

#include <QLineEdit>
#include <QTextEdit>
#include <QCheckBox>

class SetupScanerPage : public SetupPage {
private:

    QCheckBox* checkEnableLogs;
    QCheckBox* checkEnableAutoDisconnect;
    QLineEdit* editFormatDuration;
    QTextEdit* editCoinsPairs;

public:

    SetupScanerPage(QWidget* parent = nullptr) : SetupPage(parent) {}

    void createPage() override;

    void readConfig() override;


};
