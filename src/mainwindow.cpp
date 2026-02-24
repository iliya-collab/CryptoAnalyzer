#include "mainwindow.hpp"

#include <QWidgetAction>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QLabel>
#include <QDate>
#include <memory>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupEngine();
    setupUI();
    setupConnection();
}

void MainWindow::setupUI() {
    setWindowTitle("ByBit Platform");
    resize(800, 600);

    createUI();
    createMenu();
}

void MainWindow::setupConnection() {
    connect(m_actionSetupMenu, &QAction::triggered, this, [this] () {
        m_setupMenu = std::make_unique<DialogSetupMenu>(this);
    });
    connect(m_actionSaveSetup, &QAction::triggered, this, [this] () {
        Settings::writeAllConfig();
    });
    /*connect(m_actionOrderBooks, &QAction::triggered, this, [this]() {
        m_stackWidgets->setCurrentWidget(m_widgetOrderBooks);
    });*/
}

void MainWindow::setupEngine() {
    m_loader = std::make_unique<AppEngineLoader>();

    connect(m_loader.get(), &AppEngineLoader::errorEngine, this, [] (const QString& error) {
        qCritical().noquote() << error;
    });

    connect(m_loader.get(), &AppEngineLoader::finished, this, [] () {

    });

    m_loader->startDownload();
}

void MainWindow::createMenu() {
    QMenuBar* menuBar = this->menuBar();

    QMenu* menuSetup = menuBar->addMenu("Settings");
    m_actionSetupMenu = new QAction("Setup", this);
    menuSetup->addAction(m_actionSetupMenu);
    m_actionSaveSetup = new QAction("Save", this);
    menuSetup->addAction(m_actionSaveSetup);
    
    /*QMenu* menuMarket = menuBar->addMenu("Market");
    QWidgetAction* marketAction = new QWidgetAction(this);
    m_widgetMarkets = new MarketsWidget(this);
    marketAction->setDefaultWidget(m_widgetMarkets->widget());    
    menuMarket->addAction(marketAction);
    m_widgetMarkets->setAttribute(Qt::WA_TransparentForMouseEvents, true);

    QMenu* menuCoins = menuBar->addMenu("Coins");
    QWidgetAction* coinsAction = new QWidgetAction(this);
    m_widgetCoins = new CoinsWidget(this);
    coinsAction->setDefaultWidget(m_widgetCoins->widget());    
    menuCoins->addAction(coinsAction);
    m_widgetCoins->setAttribute(Qt::WA_TransparentForMouseEvents, true);

    QMenu* menuMonitoring = menuBar->addMenu("Monitoring");
    m_actionTicker = new QAction("Ticker", this);
    menuMonitoring->addAction(m_actionTicker);
    m_actionOrderBooks = new QAction("Order books", this);
    menuMonitoring->addAction(m_actionOrderBooks);*/

}

void MainWindow::createUI() {
    m_widgetMainWindow = new QWidget(this);
    setCentralWidget(m_widgetMainWindow);

    m_stackWidgets = new QStackedWidget(this);
    m_stackWidgets->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    createPages();

    m_mainLayout = new QVBoxLayout(m_widgetMainWindow);
    m_mainLayout->addWidget(m_stackWidgets);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);

}

void MainWindow::createPages() {
    /*m_widgetOrderBooks = new OrderBooksWidget(this);
    m_stackWidgets->addWidget(m_widgetOrderBooks);*/
}