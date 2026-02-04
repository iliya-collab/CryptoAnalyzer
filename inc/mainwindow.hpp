#pragma once

#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QStackedWidget>

#include "Managers/Settings.hpp"
#include "CustomWindowDialogs/DialogSetupMenu.hpp"
#include "CustomWidgets/OrderBooksWidget.hpp"
#include "CustomWidgets/MarketsWidget.hpp"
#include "CustomWidgets/CoinsWidget.hpp"
#include "Engine/AppEngine.hpp"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    ~MainWindow() = default;
    MainWindow(QWidget *parent = nullptr);

private:

    QWidget* mainWindow;
    QVBoxLayout* mainLayout;

    QStackedWidget* stackWidgets;

    std::unique_ptr<DialogSetupMenu> DSetupMenu;
    QAction* actionSetupMenu;
    QAction* actionSaveSetup;

    MarketsWidget* markets;
    CoinsWidget* coins;
    OrderBooksWidget* orderBooks;

    QAction* actionOrderBooks;
    QAction* actionTicker;

    AppEngine app_engine;

    void setupUI();
    void connectionSignals();

    void createMenu();
    void createUI();

    void setupPages();

private slots:

    void onSetupMenuActivated();
    void onSaveSetupActivated();

};
