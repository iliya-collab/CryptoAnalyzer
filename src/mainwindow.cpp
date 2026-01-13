#include "mainwindow.hpp"

#include "Managers/RestartManager.hpp"
#include "CustomWindowDialogs/DebugOutput.hpp"

#include <QWidgetAction>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QLabel>
#include <QDate>
#include <memory>


MainWindow::MainWindow(QString style, QWidget *parent) : QMainWindow(parent) {

    setWindowTitle("app");
    resize(800, 600);
    setStyleSheet(style);

    getCurrentConfig();

    setupUI();
    connectionSignals();

    DebugOutput::instance()->redirectQtMessages();
    DebugOutput::instance()->setTextEdit(outputResult);

    _scanner = std::make_unique<Scanner>(this);
    _scanner->setConfig(curScannerConfig, curMyWalletConfig);
}

MainWindow::~MainWindow() {}

void MainWindow::getCurrentConfig() {
    curScannerConfig = ScannerConfig::instance().getConfig();
    curMyWalletConfig = MyWalletConfig::instance().getConfig();
}

void MainWindow::setupUI() {
    mainWindow = new QWidget(this);
    setCentralWidget(mainWindow);

    createMenu();
    createUI();
}

void MainWindow::connectionSignals() {
    connect(btnConvert, &QPushButton::clicked, this, &MainWindow::onClickedButtonConvert);
    connect(btnStart, &QPushButton::clicked, this, &MainWindow::onClickedButtonStart);
    connect(btnEnd, &QPushButton::clicked, this, &MainWindow::onClickedButtonEnd);
    connect(btnClearOutput, &QPushButton::clicked, this, &MainWindow::onClickedButtonClearOutput);

    connect(actionSetupMenu, &QAction::triggered, this, &MainWindow::onSetupMenuActivated);
    connect(actionSaveSetup, &QAction::triggered, this, &MainWindow::onSaveSetupActivated);
    connect(actionDefaultReset, &QAction::triggered, this, &MainWindow::onDefaultResetActivated);
}

void MainWindow::createMenu() {
    QMenuBar* menuBar = this->menuBar();

    QMenu* menuSetup = menuBar->addMenu("Settings");
    actionSetupMenu = new QAction("Setup", this);
    menuSetup->addAction(actionSetupMenu);
    actionDefaultReset = new QAction("Default Reset", this);
    menuSetup->addAction(actionDefaultReset);
    actionSaveSetup = new QAction("Save", this);
    menuSetup->addAction(actionSaveSetup);

}

void MainWindow::createUI() {
    editValute = new QLineEdit(this);
    btnConvert = new QPushButton("Convert", this);
    btnStart = new QPushButton("Start", this);
    btnEnd = new QPushButton("End", this);
    btnClearOutput = new QPushButton("Clear", this);
    outputResult = new QTextEdit(this);
    outputResult->setReadOnly(true);

    mainLayout = new QGridLayout(mainWindow);
    const int Row = 10, Col = 10;
    for (int row = 0; row < Row; ++row) {
        mainLayout->setRowStretch(row, 1);
        mainLayout->setRowMinimumHeight(row, 5);
    }
    for (int col = 0; col < Col; ++col) {
        mainLayout->setColumnStretch(col, 1);
        mainLayout->setColumnMinimumWidth(col, 5);
    }
    for (int row = 0; row < Row; ++row)
        for (int col = 0; col < Col; ++col)
            mainLayout->addWidget(new QWidget(this), row, col);

    mainLayout->addWidget(editValute, 0, 0, 1, 10);
    mainLayout->addWidget(outputResult, 1, 0, 8, 10);
    mainLayout->addWidget(btnStart, 9, 0, 1, 1);
    mainLayout->addWidget(btnEnd, 9, 1, 1, 1);
    mainLayout->addWidget(btnConvert, 9, 8, 1, 1);
    mainLayout->addWidget(btnClearOutput, 9, 9, 1, 1);
}

bool MainWindow::showMessage(const char* title, const char* msg, QMessageBox::Icon icon) {
    QMessageBox msgBox(this);

    msgBox.setWindowTitle(title);
    msgBox.setText(msg);
    msgBox.setIcon(icon);

    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);

    msgBox.button(QMessageBox::Yes)->setText("Yes");
    msgBox.button(QMessageBox::No)->setText("No");

    int result = msgBox.exec();

    return (result == QMessageBox::Yes) ? true : false;
}

void MainWindow::restartApplication() {
    RestartManager::requestRestart();
}
