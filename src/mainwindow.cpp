#include "mainwindow.hpp"

#include <QWidgetAction>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QLabel>
#include <QDate>
#include <memory>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUI();
    setupConnection();
    setupEngine();
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
    Engine::AppEngineLoader* loader = new Engine::AppEngineLoader(this);

    connect(loader, &Engine::AppEngineLoader::stepStarted, [](const QString& step) {
        qDebug().noquote() << step;
    });

    connect(loader, &Engine::AppEngineLoader::progressChanged, [](int current, int total) {
        qDebug().noquote() << "Progress:" << current << "/" << total;
    });

    connect(loader, &Engine::AppEngineLoader::errorEngine, [](const QString& error) {
        qDebug().noquote() << "Error:" << error;
    });

    connect(loader, &Engine::AppEngineLoader::finished, [loader](bool success) {
        if (success) {
            qDebug().noquote() << "Loading completed successfully";
            
            auto spotPairs = loader->getData("spot");
        } else
            qDebug().noquote() << "Loading failed";
        
        loader->deleteLater();
    });

    loader->startLoading();
}

void MainWindow::createMenu() {
    m_menuBar = this->menuBar();

    QMenu* menuSetup = m_menuBar->addMenu("General");
    m_actionSetupMenu = new QAction("Setup", this);
    menuSetup->addAction(m_actionSetupMenu);
    m_actionSaveSetup = new QAction("Save", this);
    menuSetup->addAction(m_actionSaveSetup);

    QMenu* menuTrade = m_menuBar->addMenu("Trading");
    menuTrade->setFixedWidth(100);

    QMenu* menuSpot = menuTrade->addMenu("Spot");
    QWidgetAction* widgetAction = new QWidgetAction(this);
    m_widgetSpot = new SpotWidget;
    widgetAction->setDefaultWidget(m_widgetSpot);
    menuSpot->addAction(widgetAction);

    QMenu* menuFutures = menuTrade->addMenu("Futures");
    QMenu* menuOptions = menuTrade->addMenu("Options");
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