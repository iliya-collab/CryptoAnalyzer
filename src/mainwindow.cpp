#include "mainwindow.hpp"

#include <QWidgetAction>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QLabel>
#include <QDate>
#include <memory>


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("ByBit Platform");
    resize(800, 600);

    setupUI();
    connectionSignals();

    app_engine.initEngine("Bybit", "spot", "BTC/USDT");
    app_engine.runWebSocket();
}

void MainWindow::setupUI() {
    createUI();
    createMenu();
}

void MainWindow::connectionSignals() {
    connect(actionSetupMenu, &QAction::triggered, this, &MainWindow::onSetupMenuActivated);
    connect(actionSaveSetup, &QAction::triggered, this, &MainWindow::onSaveSetupActivated);

    connect(coins, &CoinsWidget::selected, this, [this] (const QString& coin) {
        app_engine.stopWebSocket();
        app_engine.changeCoin(coin);
        app_engine.runWebSocket();
    });

    connect(actionOrderBooks, &QAction::triggered, this, [this]() {
        stackWidgets->setCurrentWidget(orderBooks);
    });
}

void MainWindow::createMenu() {
    QMenuBar* menuBar = this->menuBar();

    QMenu* menuSetup = menuBar->addMenu("Settings");
    actionSetupMenu = new QAction("Setup", this);
    menuSetup->addAction(actionSetupMenu);
    actionSaveSetup = new QAction("Save", this);
    menuSetup->addAction(actionSaveSetup);
    
    QMenu* menuMarket = menuBar->addMenu("Market");
    QWidgetAction* marketAction = new QWidgetAction(this);
    markets = new MarketsWidget(this);
    marketAction->setDefaultWidget(markets->widget());    
    menuMarket->addAction(marketAction);
    markets->setAttribute(Qt::WA_TransparentForMouseEvents, true);

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
    coins->setAttribute(Qt::WA_TransparentForMouseEvents, true);

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