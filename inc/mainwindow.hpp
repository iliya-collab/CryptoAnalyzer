#pragma once

#include <QApplication>
#include <QMainWindow>

#include <QGridLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QProgressBar>

#include "Parser/ParserCB.hpp"
#include "Parser/BinanceParser.hpp"
#include "Parser/RegularParser.hpp"
#include "Parser/Scanner.hpp"

#include "CustomWindowDialogs/DialogDynamicsGraph.hpp"
#include "CustomWindowDialogs/DialogTableCurrencyRates.hpp"
#include "CustomWindowDialogs/DialogSetupMenu.hpp"
#include "CustomWindowDialogs/StatusPanel.hpp"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    ~MainWindow();
    MainWindow(QString style, QWidget *parent = nullptr);

private:

    // Центральный виджет
    QWidget* mainWindow = nullptr;
    // Главный layout
    QGridLayout* mainLayout = nullptr;

    // -------------------------------------------------
    // Основные виджеты
    QLineEdit* editValute = nullptr;
    QPushButton* btnConvert = nullptr;
    QPushButton* btnStart = nullptr;
    QPushButton* btnEnd = nullptr;
    QPushButton* btnClearOutput = nullptr;
    QTextEdit* outputResult = nullptr;
    QProgressBar* progressBar = nullptr;
    // -------------------------------------------------

    // -------------------------------------------------
    // Дополнительные диалоговые окна
    DTableCurrencyRates* DTable = nullptr;
    QAction* actionTabelCurrencyRates = nullptr;

    DDynamicsGraph* DGraph = nullptr;
    QAction* actionDynamicsGraph = nullptr;

    DialogSetupMenu* DSetupMenu = nullptr;
    QAction* actionSetupMenu = nullptr;
    QAction* actionSaveSetup = nullptr;
    QAction* actionDefaultReset = nullptr;

    //StatusPanel* status_panel = nullptr;
    // -------------------------------------------------

    // -------------------------------------------------
    // Объявление парсеров
    std::unique_ptr<ParserCB> parser_cb;
    std::unique_ptr<RegularParser> reg_parser;
    std::unique_ptr<WebSocketParser> wsParser;
    // -------------------------------------------------

    std::unique_ptr<Scanner> _scanner;

    // -------------------------------------------------
    ParamsScannerConfig curScannerConfig;
    ParamsMyWalletConfig curMyWalletConfig;
    // -------------------------------------------------

    // -------------------------------------------------
    // Таймер обновления ProgressBar
    std::unique_ptr<QTimer> updateProgressBarTimer;
    // -------------------------------------------------

    void setupUI();
    void connectionSignals();

    void getCurrentConfig();
    void initParsers();
    void setupTimers();
    void setupProgressBar();

    void createMenu();
    void createUI();

    bool showMessage(const char* title, const char* msg, QMessageBox::Icon icon);

private slots:

    void onClickedButtonConvert();
    void onClickedButtonStart();
    void onClickedButtonEnd();
    void onClickedButtonClearOutput();

    void onCurrencyRatesActivated();
    void onDynamicsGraphActivated();

    void onSetupMenuActivated();
    void onSaveSetupActivated();
    void onDefaultResetActivated();

    void onUpdateProgressBar();
    void simulateProgress();
    void resetProgress();

};