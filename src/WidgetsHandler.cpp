#include "mainwindow.hpp"
#include "Managers/Settings.hpp"
#include <memory>


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

void MainWindow::onSetupMenuActivated() {
    DSetupMenu = std::make_unique<DialogSetupMenu>(this);
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
