#pragma once

#include "SetupPage.hpp"
#include "Configs/PlatformConfig.hpp"

#include <QTextEdit>

class SetupPlatformPage : public SetupPage {
private:

    QTextEdit* editCoinsPairs;
    
    ParamsPlatformConfig& curConfig;

public:

    SetupPlatformPage(QWidget* parent = nullptr) : SetupPage(parent), curConfig(PlatformConfig::instance().getConfig()) {}

    void createPage() override;

    void readConfig() override;
    
};
