#include "mainwindow.hpp"

#include "Configs/ScannerConfig.hpp"

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
    initParsers();
    setupTimers();
    connectionSignals();
    status_panel = new StatusPanel(this);

    statusBar()->showMessage("Waitting...");
}

MainWindow::~MainWindow() {}

void MainWindow::getCurrentConfig() {
    curScannerConfig = ScannerConfig::instance().getConfig();
    curMyWalletConfig = MyWalletConfig::instance().getConfig();
}

void MainWindow::setupTimers() {
    disconnectTimer = std::make_unique<QTimer>();
    disconnectTimer->setSingleShot(true); // Таймер сработает только один раз

    updateProgressBarTimer = std::make_unique<QTimer>();
    qint64 duration = curScannerConfig.allDuration;
    qint64 interval = duration / 100;
    // Проверка корректности интервала
    if (interval <= 0) {
        interval = 100; // значение по умолчанию
        qWarning() << "Incorrect timer interval, default value set to 100 ms";
    }
    updateProgressBarTimer->setInterval(interval);

    triggeredRuleTimer = std::make_unique<QTimer>();
    triggeredRuleTimer->setSingleShot(true);
}

void MainWindow::setupUI() {
    mainWindow = new QWidget(this);
    setCentralWidget(mainWindow);

    createMenu();
    createUI();
}

void MainWindow::connectionSignals() {
    connect(disconnectTimer.get(), &QTimer::timeout, this, &MainWindow::onDisconnectTimeout);
    connect(triggeredRuleTimer.get(), &QTimer::timeout, this, &MainWindow::onTriggeredRuleTimer);
    connect(updateProgressBarTimer.get(), &QTimer::timeout, this, &MainWindow::onUpdateProgressBar);

    connect(btnConvert, &QPushButton::clicked, this, &MainWindow::onClickedButtonConvert);
    connect(btnStart, &QPushButton::clicked, this, &MainWindow::onClickedButtonStart);
    connect(btnStart, &QPushButton::clicked, this, &MainWindow::simulateProgress);
    connect(btnEnd, &QPushButton::clicked, this, &MainWindow::onClickedButtonEnd);
    connect(btnEnd, &QPushButton::clicked, this, &MainWindow::resetProgress);
    connect(btnClearOutput, &QPushButton::clicked, this, &MainWindow::onClickedButtonClearOutput);

    connect(wsParser.get(), &WebSocketParser::priceUpdated, this, &MainWindow::onPriceUpdated);
    connect(wsParser.get(), &WebSocketParser::connected, this, &MainWindow::onWebSocketConnected);
    connect(wsParser.get(), &WebSocketParser::disconnected, this, &MainWindow::onWebSocketDisconnected);
    connect(wsParser.get(), &WebSocketParser::errorOccurred, this, &MainWindow::onWebSocketError);

    connect(actionTabelCurrencyRates, &QAction::triggered, this, &MainWindow::onCurrencyRatesActivated);
    connect(actionDynamicsGraph, &QAction::triggered, this, &MainWindow::onDynamicsGraphActivated);
    connect(actionSetupMenu, &QAction::triggered, this, &MainWindow::onSetupMenuActivated);
    connect(actionSaveSetup, &QAction::triggered, this, &MainWindow::onSaveSetupActivated);
    connect(actionDefaultReset, &QAction::triggered, this, &MainWindow::onDefaultResetActivated);
}

void MainWindow::initParsers() {

    parser_cb = std::make_unique<ParserCB>();
    reg_parser = std::make_unique<RegularParser>();
    my_wallet = std::make_unique<ParserMyWallet>();
    wsParser = std::make_unique<BinanceParser>();

    // Получаем курс валют ЦБ от текущей дате
    parser_cb->makeRequest();
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

    QMenu* menuWindow = menuBar->addMenu("Windows");
    actionTabelCurrencyRates = new QAction("Currency rates", this);
    menuWindow->addAction(actionTabelCurrencyRates);
    actionDynamicsGraph = new QAction("Dynamics graph", this);
    menuWindow->addAction(actionDynamicsGraph);

}

void MainWindow::setupProgressBar() {
    progressBar = new QProgressBar(this);
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    progressBar->setTextVisible(true);
    progressBar->setFormat("%p%");
    progressBar->setFixedWidth(200);

    statusBar()->addPermanentWidget(progressBar);
}

void MainWindow::createUI() {
    editValute = new QLineEdit(this);
    btnConvert = new QPushButton("Convert", this);
    btnStart = new QPushButton("Start", this);
    btnEnd = new QPushButton("End", this);
    btnClearOutput = new QPushButton("Clear", this);
    outputResult = new QTextEdit(this);
    outputResult->setReadOnly(true);
    setupProgressBar();

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
