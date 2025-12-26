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
#include "Parser/ParserMyWallet.hpp"

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

    StatusPanel* status_panel = nullptr;
    // -------------------------------------------------

    // -------------------------------------------------
    // Объявление парсеров
    std::unique_ptr<ParserCB> parser_cb;
    std::unique_ptr<RegularParser> reg_parser;
    std::unique_ptr<ParserMyWallet> my_wallet;
    std::unique_ptr<WebSocketParser> wsParser;
    // -------------------------------------------------

    // -------------------------------------------------
    ParamsScannerConfig curScannerConfig;
    ParamsMyWalletConfig curMyWalletConfig;
    // -------------------------------------------------

    // -------------------------------------------------
    // Таймер автоматического отключения сокета
    std::unique_ptr<QTimer> disconnectTimer;
    // Таймер обновления ProgressBar
    std::unique_ptr<QTimer> updateProgressBarTimer;
    // Таймер запуска правил
    std::unique_ptr<QTimer> triggeredRuleTimer;
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

    void onDisconnectTimeout();
    void onUpdateProgressBar();
    void onTriggeredRuleTimer();

    void simulateProgress();
    void resetProgress();

    // Подключаются к BinanceWebSocketParser
    // -------------------------------------------------
    // Вызывается при обновлении цены на подписанные монеты
    void onPriceUpdated(const QString &coin, double price);
    // Вызывается при подключении к websocket к серверу
    void onWebSocketConnected();
    // Вызывается при отключении websocket от серверу
    void onWebSocketDisconnected();
    // Вызывается при возникновении ошибок в подключении
    void onWebSocketError(const QString &error);
    // -------------------------------------------------

};