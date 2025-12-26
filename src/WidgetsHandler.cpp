#include "mainwindow.hpp"
#include "Settings.hpp"


void MainWindow::onClickedButtonConvert() {
}

void MainWindow::onClickedButtonStart() {
    // Подписываемся на монеты
    wsParser->subscribeToCoins(curScannerConfig.pairs);
    wsParser->connectToStream(BinanceParser::TradeUrl);

    // Запускаем таймер для автоматического отключения (если включено в конфиге)
    if (curScannerConfig.enableAutoDisconnect)
        disconnectTimer->start(curScannerConfig.allDuration);
    // Запускаем таймер для обработки правил (если есть хотя бы одно сканирование)
    if (curScannerConfig.durations.size() > 0)
        triggeredRuleTimer->start(curScannerConfig.durations[0]);

    status_panel->show();
}

void MainWindow::onClickedButtonEnd() {

    if (disconnectTimer->isActive())
        disconnectTimer->stop();

    wsParser->disconnectFromStream();
    status_panel->close();
}

void MainWindow::onDisconnectTimeout()
{
    wsParser->disconnectFromStream();
    showMessage("Auto Disconnect", "Stream automatically disconnected after timeout", QMessageBox::Information);
}

void MainWindow::onTriggeredRuleTimer() {
    static int nScan = 2;

    if (!triggeredRuleTimer->isActive() && curScannerConfig.durations.size() >= nScan) {
        triggeredRuleTimer->start(curScannerConfig.durations[nScan-1]);
        nScan++;
    }

    auto exp = my_wallet->parseAllRules();
    if (exp.has_value())
        status_panel->displayResults(exp.value());
    else
        outputResult->append(QString("Error : %1").arg(exp.error()));
}

void MainWindow::onClickedButtonClearOutput() {
    outputResult->clear();
}

void MainWindow::onCurrencyRatesActivated() {
    //DTable = new DTableCurrencyRates(5, parser_cb->getCurrencyRates().size(), parser_cb->getLastDate(), parser_cb->getCurrencyRates(), this);
}

void MainWindow::onDynamicsGraphActivated() {
    //DGraph = new DDynamicsGraph(this);
}

void MainWindow::onSetupMenuActivated() {
    DSetupMenu = new DialogSetupMenu(this);
}

void MainWindow::onSaveSetupActivated() {
    if (showMessage("Confirmation", "Save changes", QMessageBox::Question))
        Settings::writeAllConfig();
    if (showMessage("Confirmation", "Restart the progremm?", QMessageBox::Question)) {
        // Закрываем текущее приложение
        QApplication::quit();
        // Запускаем новое
        QProcess::startDetached(QApplication::applicationFilePath(), QApplication::arguments());
    }
}

void MainWindow::onDefaultResetActivated() {
    MyWalletConfig::instance().setDefaultConfig();
    ScannerConfig::instance().setDefaultConfig();
}


void MainWindow::simulateProgress() {
    if (updateProgressBarTimer && updateProgressBarTimer->isActive())
        updateProgressBarTimer->stop();

    progressBar->setValue(0);
    statusBar()->showMessage("In progress...");

    if (updateProgressBarTimer) {
        updateProgressBarTimer->start();
    }
}

void MainWindow::resetProgress() {
    if (updateProgressBarTimer && updateProgressBarTimer->isActive())
        updateProgressBarTimer->stop();

    if (progressBar)
        progressBar->setValue(0);

    statusBar()->showMessage("Dropped");
}

void MainWindow::onUpdateProgressBar() {
    if (!progressBar)
        return;

    int currentValue = progressBar->value();
    int newValue = currentValue + 1;

    progressBar->setValue(newValue);

    // Проверяем завершение
    if (newValue >= 100) {
        if (updateProgressBarTimer && updateProgressBarTimer->isActive())
            updateProgressBarTimer->stop();
        statusBar()->showMessage("Completed", 3000);
    }
}

void MainWindow::onPriceUpdated(const QString &coin, double price)
{
    // Вызывается мгновенно при изменении цены на Binance
    my_wallet->updateAllAssets(coin, price);
    my_wallet->updateWallet();

    status_panel->display();
}

void MainWindow::onWebSocketConnected() {
    outputResult->append("Connected to Binance WebSocket");
}

void MainWindow::onWebSocketDisconnected() {
    outputResult->append("Disconnected from Binance WebSocket");
}

void MainWindow::onWebSocketError(const QString &error) {
   outputResult->append("WebSocket error: " + error);
}
