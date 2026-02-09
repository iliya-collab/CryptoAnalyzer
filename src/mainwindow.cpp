#include "mainwindow.hpp"

#include <QWidgetAction>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QLabel>
#include <QDate>
#include <memory>

#include "Engine/Downloader.hpp"

MainWindow::MainWindow(std::unique_ptr<AppEngine> app_engine, QWidget *parent) : 
    QMainWindow(parent),
    m_engine(std::move(app_engine))
{
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
        m_setupMenu = std::make_unique<DialogSetupMenu>(this);
    });
    connect(m_actionSaveSetup, &QAction::triggered, this, [this] () {
        Settings::writeAllConfig();
    });
    connect(m_actionOrderBooks, &QAction::triggered, this, [this]() {
        m_stackWidgets->setCurrentWidget(m_widgetOrderBooks);
    });
}

void MainWindow::setupEngine() {
    connect(m_engine.get(), &AppEngine::spotReady, this, [this] (const QStringList& coins) {
        qDebug() << "AppEngine::spotReady";
        m_widgetCoins->updateWidget(coins);
        m_engine->getInfoAboutCoins();
    });
    connect(m_engine.get(), &AppEngine::futuresReady, this, [this] (const QStringList& coins) {
        qDebug() << "AppEngine::futuresReady";
        m_widgetCoins->updateWidget(coins);
        m_engine->getInfoAboutCoins();
    });

    connect(m_engine.get(), &AppEngine::infoAboutCoinsReady, this, [this] (const QHash<QString, Engine::InfoAboutCoin>& lstCoins) {
        qDebug() << "AppEngine::infoAboutCoinsReady";
    });
    connect(m_engine.get(), &AppEngine::infoAboutIconsReady, this, [this] (const QHash<QString, QString>& icons) {
        qDebug() << "AppEngine::infoAboutIconsReady";
        m_engine->downloadAllIcons(icons);
    });

    m_engine->getSpotTradingPairs();
}

void MainWindow::createMenu() {
    QMenuBar* menuBar = this->menuBar();

    QMenu* menuSetup = menuBar->addMenu("Settings");
    m_actionSetupMenu = new QAction("Setup", this);
    menuSetup->addAction(m_actionSetupMenu);
    m_actionSaveSetup = new QAction("Save", this);
    menuSetup->addAction(m_actionSaveSetup);
    
    QMenu* menuMarket = menuBar->addMenu("Market");
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
    menuMonitoring->addAction(m_actionOrderBooks);

}

void MainWindow::createUI() {
    m_widgetMainWindow = new QWidget(this);
    setCentralWidget(m_widgetMainWindow);

    m_stackWidgets = new QStackedWidget(this);
    m_stackWidgets->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    createPages();
/*
    // Загружаем изображение
    QPixmap pixmap("btc.png");
    
    // Создаем QLabel и устанавливаем в него изображение
    QLabel *imageLabel = new QLabel(this);
    imageLabel->setPixmap(pixmap);
    
    // Опционально: масштабирование
    imageLabel->setPixmap(pixmap.scaled(50, 50, Qt::KeepAspectRatio));
    
    // Центрируем изображение
    imageLabel->setAlignment(Qt::AlignCenter);*/

    m_mainLayout = new QVBoxLayout(m_widgetMainWindow);
    //m_mainLayout->addWidget(imageLabel);
    m_mainLayout->addWidget(m_stackWidgets);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);

}

void MainWindow::createPages() {
    m_widgetOrderBooks = new OrderBooksWidget(this);
    m_stackWidgets->addWidget(m_widgetOrderBooks);
}