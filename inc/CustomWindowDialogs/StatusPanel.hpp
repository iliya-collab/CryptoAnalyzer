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

    QTextEdit* outputStatAssets;
    QTextEdit* outputStatWallet;
    QTextEdit* outputMessages;

    QPushButton* btnOK;

    // Обновляет содержимое монет
    void displayEachAsset();
    // Обновляет содержимое кошелька
    void displayMyWallet();

public:

    enum Display {
        StatAssets,
        StatWallet,
        Messages
    };

    void displayStatWallet();
    void displayStatAssets();
    void displayMessages(const QString& msg);

    void outputStringInDisplay(Display _disp, const QString& str);

    void clearDisplay(Display _disp);

    void show();

    StatusPanel(QWidget* parent = nullptr);
};
