#pragma once

#include <QApplication>
#include <QMainWindow>

#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QMessageBox>

#include <QRadioButton>
#include <QButtonGroup>

#include "Managers/Settings.hpp"

#include "Parser/Scanner.hpp"
#include "CustomWindowDialogs/ViewerOrderBooksController.hpp"
#include "CustomWindowDialogs/DialogSetupMenu.hpp"
#include "CustomWindowDialogs/DebugMonitor.hpp"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    ~MainWindow() = default;
    MainWindow(QWidget *parent = nullptr);

private:

    QWidget* mainWindow;
    QVBoxLayout* mainLayout;

    std::unique_ptr<DebugMonitor> debug;

    std::unique_ptr<DialogSetupMenu> DSetupMenu;
    QAction* actionSetupMenu;
    QAction* actionSaveSetup;
    QAction* actionDefaultReset;

    std::unique_ptr<Scanner> m_scanner;
    
    enum IDButtons {
        IdRadioSpotMarket,
        IdRadioFuturesMarket
    };

    QButtonGroup* groupMarkets;
    QWidget* createWidgetMenuMarket();

    QComboBox* comboCoins;
    QWidget* createWidgetMenuCoins();

    std::unique_ptr<ViewerOrderBooksController> m_crtl_ord_books;
    QAction* actionOrderBooks;
    QAction* actionTicker;

    void setupUI();
    void connectionSignals();

    void createMenu();
    void createUI();

private slots:

    void onSetupMenuActivated();
    void onSaveSetupActivated();
    void onDefaultResetActivated();

};
