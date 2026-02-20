#pragma once

#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QStackedWidget>

#include "CustomWindowDialogs/DialogSetupMenu.hpp"
#include "CustomWidgets/OrderBooksWidget.hpp"
#include "CustomWidgets/MarketsWidget.hpp"
#include "CustomWidgets/CoinsWidget.hpp"
#include "Engine/App/AppEngineLoader.hpp"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    ~MainWindow() = default;
    MainWindow(QWidget *parent = nullptr);

private:

    QWidget* m_widgetMainWindow;
    QVBoxLayout* m_mainLayout;

    QStackedWidget* m_stackWidgets;

    std::unique_ptr<DialogSetupMenu> m_setupMenu;
    
    MarketsWidget* m_widgetMarkets;
    CoinsWidget* m_widgetCoins;
    OrderBooksWidget* m_widgetOrderBooks;
    
    QAction* m_actionSetupMenu;
    QAction* m_actionSaveSetup;
    QAction* m_actionOrderBooks;
    QAction* m_actionTicker;

    std::unique_ptr<AppEngineLoader> m_loader;

    void setupUI();
    void setupConnection();
    void setupEngine();

    void createMenu();
    void createUI();
    void createPages();

};
