#pragma once

#include <QApplication>
#include <QMainWindow>

#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QMessageBox>

#include <QStackedWidget>

#include "Managers/Settings.hpp"

//#include "Parser/Scanner.hpp"
#include "CustomWindowDialogs/DialogSetupMenu.hpp"
#include "CustomWindowDialogs/DebugMonitor.hpp"

#include "CustomWidgets/OrderBooksWidget.hpp"
#include "CustomWidgets/MarketsWidget.hpp"
#include "CustomWidgets/CoinsWidget.hpp"

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

    std::unique_ptr<DebugMonitor> debug;

    std::unique_ptr<DialogSetupMenu> DSetupMenu;
    QAction* actionSetupMenu;
    QAction* actionSaveSetup;
    QAction* actionDefaultReset;

    //std::unique_ptr<Scanner> m_scanner;

    MarketsWidget* markets;
    CoinsWidget* coins;
    OrderBooksWidget* orderBooks;

    QAction* actionOrderBooks;
    QAction* actionTicker;

    void setupUI();
    void connectionSignals();

    void createMenu();
    void createUI();

    void setupPages();

private slots:

    void onSetupMenuActivated();
    void onSaveSetupActivated();
    void onDefaultResetActivated();

};
