#pragma once

#include <QApplication>
#include <QMainWindow>

#include <QGridLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QMenuBar>
#include <QMenu>
#include <QAction>

#include "CustomWindowDialogs/DialogSetupMenu.hpp"
#include "CustomWindowDialogs/DialogScanner.hpp"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    ~MainWindow();
    MainWindow(QString style, QWidget *parent = nullptr);

private:

    QWidget* mainWindow = nullptr;
    QGridLayout* mainLayout = nullptr;

    // -------------------------------------------------
    QPushButton* btnStart = nullptr;
    QPushButton* btnEnd = nullptr;
    QPushButton* btnClearOutput = nullptr;
    QTextEdit* outputResult = nullptr;
    // -------------------------------------------------

    // -------------------------------------------------
    std::unique_ptr<DialogSetupMenu> DSetupMenu = nullptr;
    QAction* actionSetupMenu = nullptr;
    QAction* actionSaveSetup = nullptr;
    QAction* actionDefaultReset = nullptr;
    // -------------------------------------------------

    std::unique_ptr<DialogScanner> DScanner;

    // -------------------------------------------------
    ParamsScannerConfig curScannerConfig;
    ParamsMyWalletConfig curMyWalletConfig;
    // -------------------------------------------------

    void setupUI();
    void connectionSignals();

    void getCurrentConfig();

    void createMenu();
    void createUI();

    bool showMessage(const char* title, const char* msg, QMessageBox::Icon icon);
    void restartApplication();

private slots:

    void onClickedButtonStart();
    void onClickedButtonEnd();
    void onClickedButtonClearOutput();

    void onSetupMenuActivated();
    void onSaveSetupActivated();
    void onDefaultResetActivated();

};
