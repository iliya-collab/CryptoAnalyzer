#pragma once

#include "SetupPage.hpp"
#include "Configs/ScannerConfig.hpp"

#include <QLineEdit>
#include <QTextEdit>
#include <QCheckBox>
#include <QPushButton>

class SetupScanerPage : public SetupPage {
private:


    QTextEdit* editCoinsPairs;
    
    ParamsScannerConfig& curConfig;

public:

    SetupScanerPage(QWidget* parent = nullptr) : SetupPage(parent), curConfig(ScannerConfig::instance().getConfig()) {}

    void createPage() override;

    void readConfig() override;


};
