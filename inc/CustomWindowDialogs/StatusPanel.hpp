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
    QPushButton* btnOK;

    // Обновляет содержимое монет
    void displayEachAsset();
    // Обновляет содержимое кошелька
    void displayMyWallet();

public:

    void displayResults(double val);
    // Обновляет содежимое всего вывода
    void display();
    // Вызывает отображение виджета
    void show();

    StatusPanel(QWidget* parent = nullptr);
};
