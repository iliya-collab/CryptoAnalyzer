#pragma once

#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QStackedWidget>

#include "CustomWindowDialogs/DialogSetupMenu.hpp"
#include "Engine/App/AppEngineLoader.hpp"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:

    ~MainWindow() = default;
    MainWindow(QWidget *parent = nullptr);

private:

    QWidget* m_widgetMainWindow;
    QVBoxLayout* m_mainLayout;

    QStackedWidget* m_stackWidgets;

    DialogSetupMenu* m_dlgSetupMenu;
    
    QAction* m_actionSetupMenu;
    QAction* m_actionSaveSetup;

    QMenuBar* m_menuBar;

    std::unique_ptr<AppEngineLoader> m_loader;

    void setupUI();
    void setupConnection();
    void setupEngine();

    void createMenu();
    void createUI();
    void createPages();

};
