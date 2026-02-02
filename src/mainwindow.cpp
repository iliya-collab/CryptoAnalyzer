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

    m_scanner = std::make_unique<Scanner>();
    m_scanner->setConfig(PlatformConfig::instance().getConfig());

}

void MainWindow::setupUI() {
    mainWindow = new QWidget(this);
    setCentralWidget(mainWindow);

    createUI();
    createMenu();
}

void MainWindow::connectionSignals() {
    connect(actionSetupMenu, &QAction::triggered, this, &MainWindow::onSetupMenuActivated);
    connect(actionSaveSetup, &QAction::triggered, this, &MainWindow::onSaveSetupActivated);
    connect(actionDefaultReset, &QAction::triggered, this, &MainWindow::onDefaultResetActivated);

    connect(actionOrderBooks, &QAction::triggered, this, [this]() { 
        m_crtl_ord_books->create();
        m_crtl_ord_books->show();
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
    marketAction->setDefaultWidget(createWidgetMenuMarket());    
    menuMarket->addAction(marketAction);

    QMenu* menuMonitoring = menuBar->addMenu("Monitoring");
    actionTicker = new QAction("Ticker", this);
    menuMonitoring->addAction(actionTicker);
    actionOrderBooks = new QAction("Order books", this);
    menuMonitoring->addAction(actionOrderBooks);

}

void MainWindow::createUI() {
    mainWindow = new QWidget(this);
    setCentralWidget(mainWindow);

    mainLayout = new QVBoxLayout(mainWindow);
}

QWidget* MainWindow::createWidgetMenuMarket() {
    QWidget* widgetMenuMarket = new QWidget(this);
    QVBoxLayout* layoutMenuMarket = new QVBoxLayout(widgetMenuMarket);

    QRadioButton* radioSpotMarket = new QRadioButton("Spot", this);
    QRadioButton* radioFuturesMarket = new QRadioButton("Futures", this);

    layoutMenuMarket->addWidget(radioSpotMarket);
    layoutMenuMarket->addWidget(radioFuturesMarket);
    
    groupMarkets = new QButtonGroup(this);
    groupMarkets->addButton(radioSpotMarket, IDButtons::IdRadioSpotMarket);
    groupMarkets->addButton(radioFuturesMarket, IDButtons::IdRadioFuturesMarket);
    radioSpotMarket->setChecked(true);

    return widgetMenuMarket;
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