#pragma once

#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QStackedWidget>
#include <QProgressBar>
#include <QStatusBar>

#include "CustomWindowDialogs/DialogSetupMenu.hpp"
#include "Engine/App/AppEngineLoader.hpp"
#include "Managers/Settings.hpp"

#include "CustomWidgets/SpotWidget.hpp"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:

    ~MainWindow() = default;
    MainWindow(QWidget *parent = nullptr);

private:

    QWidget* m_widgetMainWindow;
    QVBoxLayout* m_mainLayout;
    QProgressBar* m_progressBar;

    QStackedWidget* m_stackWidgets;

    DialogSetupMenu* m_dlgSetupMenu;
    
    QAction* m_actionSetupMenu;
    QAction* m_actionSaveSetup;

    SpotWidget* m_widgetSpot;

    QMenuBar* m_menuBar;

    void setupUI();
    void setupConnection();
    void setupEngine();
    
    void createMenu();
    void createUI();
    void createStatusBar();
    void createPages();

};
