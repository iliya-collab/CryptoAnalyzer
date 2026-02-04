#pragma once

#include "SetupPage.hpp"
#include "Configs/PlatformConfig.hpp"

#include <QComboBox>
#include <QLabel>

class SetupAPIKeyPage : public SetupPage {
private:

    QComboBox* select_key;
    QLabel* api_key;
    QLabel* secret_key;
    
    ParamsPlatformConfig& curConfig;

public:

    SetupAPIKeyPage(QWidget* parent = nullptr) : SetupPage(parent), curConfig(PlatformConfig::instance().getConfig()) {}

    void createPage() override;

    void readConfig() override;


};