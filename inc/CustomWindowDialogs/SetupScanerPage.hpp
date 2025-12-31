#pragma once

#include "SetupPage.hpp"
#include "Configs/ScannerConfig.hpp"

#include <QLineEdit>
#include <QTextEdit>
#include <QCheckBox>
#include <QPushButton>

#include "CustomWidgets/EditableComboBox.hpp"

class SetupScanerPage : public SetupPage {
private:

    QCheckBox* checkEnableLogs;
    QCheckBox* checkEnableAutoDisconnect;

    QTextEdit* editCoinsPairs;
    QTextEdit* editRules;
    
    EditableComboBox* comboDurations;

    QMap<QString, QString> parseRules();

    ParamsScannerConfig& curConfig;

private slots:
    void onComboIndexChanged(const QString& text, int index);
    void onEditingFinished(const QString& text);

public:

    SetupScanerPage(QWidget* parent = nullptr) : SetupPage(parent), curConfig(ScannerConfig::instance().getConfig()) {}

    void createPage() override;

    void readConfig() override;


};
