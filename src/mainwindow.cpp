#include "mainwindow.hpp"

#include <QWidgetAction>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QLabel>
#include <QDate>
#include <memory>

#include "CustomWidgets/TradeMenu.hpp"

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
        m_dlgSetupMenu = new DialogSetupMenu(this);
    });
    connect(m_actionSaveSetup, &QAction::triggered, this, [this] () {
        Settings::writeAllConfig();
    });
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
    m_menuBar = this->menuBar();

    QMenu* menuSetup = m_menuBar->addMenu("General");
    m_actionSetupMenu = new QAction("Setup", this);
    menuSetup->addAction(m_actionSetupMenu);
    m_actionSaveSetup = new QAction("Save", this);
    menuSetup->addAction(m_actionSaveSetup);

    TradeMenu *tradingMenu = new TradeMenu(this);
    tradingMenu->setTitle("Trading");
    m_menuBar->addMenu(tradingMenu);

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
}