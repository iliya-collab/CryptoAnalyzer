#pragma once

#include "CustomWindowDialogs/CustomQDialog.hpp"

#include <QWidget>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMap>
#include <QScrollBar>
#include <QFrame>
#include <QTextTable>
#include <QSplitter>

// Класс описывает окно, которое вызывается при начале сканирования и показавает состаяние твоего кошелька
class StatusPanel : public CustomQDialog
{
    Q_OBJECT
private:

    void setupUI();
    void connectionSignals();

    void showWidget();

    QTextEdit* outputDynamic;
    QTextEdit* outputResults;
    QTextEdit* outputMessage;

    QPushButton* btnOK;

    QSplitter* verSplitter;
    QSplitter* horSplitter;
    

    // Обновляет содержимое монет
    void displayEachAsset();
    // Обновляет содержимое кошелька
    void displayMyWallet();

public:

    void displayResults(double val);
    void displayDynamic();
    void displayMessage(const QString& msg);

    void show();

    StatusPanel(QWidget* parent = nullptr);
};
