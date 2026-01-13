#pragma once

#include <QApplication>
#include <QMainWindow>

#include <QGridLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QMenuBar>
#include <QMenu>
#include <QAction>

#include "CustomWindowDialogs/DialogSetupMenu.hpp"

#include "Parser/Scanner.hpp"

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
    QLineEdit* editValute = nullptr;
    QPushButton* btnConvert = nullptr;
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

    std::unique_ptr<DialogTable> DTable;
    QAction* actionTabel = nullptr;

    std::unique_ptr<DDynamicsGraph> DGraph;
    QAction* actionDynamicsGraph = nullptr;
    // -------------------------------------------------

    std::unique_ptr<Scanner> _scanner;

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

    void onClickedButtonConvert();
    void onClickedButtonStart();
    void onClickedButtonEnd();
    void onClickedButtonClearOutput();

    void onSetupMenuActivated();
    void onSaveSetupActivated();
    void onDefaultResetActivated();

};
