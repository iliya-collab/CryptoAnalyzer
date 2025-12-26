#include "mainwindow.hpp"
#include "Settings.hpp"


void MainWindow::onClickedButtonConvert() {
}

void MainWindow::onClickedButtonStart() {
    _scanner->start();
}

void MainWindow::onClickedButtonEnd() {
    _scanner->stop();
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
    if (showMessage("Confirmation", "Restart the progremm?", QMessageBox::Question))
        restartApplication();
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
