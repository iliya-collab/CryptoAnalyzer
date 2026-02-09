#pragma once

#include "SetupPage.hpp"
#include "Configs/PlatformConfig.hpp"

#include <QComboBox>
#include <QLabel>

class SetupAPIPage : public SetupPage {
private:

    QComboBox* select_key;
    QLabel* api_key;
    QLabel* secret_key;
    
    ParamsPlatformConfig& curConfig;

public:

    SetupAPIPage(QWidget* parent = nullptr) : SetupPage(parent), curConfig(PlatformConfig::instance().getConfig()) {}

    void createPage() override;

    void readConfig() override;


};