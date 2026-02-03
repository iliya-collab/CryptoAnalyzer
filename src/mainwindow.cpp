#include "mainwindow.hpp"

#include <QWidgetAction>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QLabel>
#include <QDate>
#include <memory>


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("ByBit Trading Platform");
    resize(800, 600);

    setupUI();
    connectionSignals();

    debug = std::make_unique<DebugMonitor>(this);
    debug->show();

    qDebug() << "--- Reading the platform configuration ---";
    if (!Settings::readAllConfig())
        qDebug() << Settings::getLastError();
    qDebug() << "--- Configuration reading is completed ---";

    //m_scanner = std::make_unique<Scanner>();
    //m_scanner->setConfig(PlatformConfig::instance().getConfig());

}

void MainWindow::setupUI() {
    createUI();
    createMenu();
}

void MainWindow::connectionSignals() {
    connect(actionSetupMenu, &QAction::triggered, this, &MainWindow::onSetupMenuActivated);
    connect(actionSaveSetup, &QAction::triggered, this, &MainWindow::onSaveSetupActivated);
    connect(actionDefaultReset, &QAction::triggered, this, &MainWindow::onDefaultResetActivated);

    connect(actionOrderBooks, &QAction::triggered, this, [this]() {
        stackWidgets->setCurrentWidget(orderBooks);
    });
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
    
    QMenu* menuMarket = menuBar->addMenu("Market");
    QWidgetAction* marketAction = new QWidgetAction(this);
    markets = new MarketsWidget(this);
    marketAction->setDefaultWidget(markets->widget());    
    menuMarket->addAction(marketAction);

    QMenu* menuCoins = menuBar->addMenu("Coins");
    QWidgetAction* coinsAction = new QWidgetAction(this);
    coins = new CoinsWidget(this);
    QStringList coinsData = {
        "BTC/USDT",
        "ETH/USDT",
        "ADA/USDT",
        "DOT/USDT",
        "SOL/USDT", 
        "XRP/USDT",
        "DOGE/USDT"
    };
    coins->setList(coinsData);
    coinsAction->setDefaultWidget(coins->widget());    
    menuCoins->addAction(coinsAction);

    QMenu* menuMonitoring = menuBar->addMenu("Monitoring");
    actionTicker = new QAction("Ticker", this);
    menuMonitoring->addAction(actionTicker);
    actionOrderBooks = new QAction("Order books", this);
    menuMonitoring->addAction(actionOrderBooks);

}

void MainWindow::createUI() {
    mainWindow = new QWidget(this);
    setCentralWidget(mainWindow);

    stackWidgets = new QStackedWidget(this);
    stackWidgets->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    //stackWidgets->setStyleSheet("QStackedWidget { background-color: #f0f0f0; border: 1px solid red; }");
    setupPages();

    mainLayout = new QVBoxLayout(mainWindow);
    mainLayout->addWidget(stackWidgets);
    mainLayout->setContentsMargins(0, 0, 0, 0);

}

void MainWindow::setupPages() {
    orderBooks = new OrderBooksWidget(this);

    stackWidgets->addWidget(orderBooks);
}

void MainWindow::onSetupMenuActivated() {
    DSetupMenu = std::make_unique<DialogSetupMenu>(this);
}

void MainWindow::onSaveSetupActivated() {
    Settings::writeAllConfig();
}

void MainWindow::onDefaultResetActivated() {
    PlatformConfig::instance().setDefaultConfig();
}