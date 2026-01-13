#pragma once

#include "CustomWindowDialogs/CustomQDialog.hpp"
#include "CustomWindowDialogs/DialogDynamicsGraph.hpp"
#include "CustomWindowDialogs/DialogTable.hpp"

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
#include <QMenuBar>
#include <QMenu>
#include <QAction>

class StatusPanel : public CustomQDialog {
    Q_OBJECT
private:

    void setupUI();
    void connectionSignals();
    void setupMenu();

    QTextEdit* outputStatAssets;
    QTextEdit* outputStatWallet;
    QTextEdit* outputMessages;

    QPushButton* btnOK;

    std::unique_ptr<TableController> table;
    QAction* actionTabel = nullptr;

    std::unique_ptr<DDynamicsGraph> DGraph;
    QAction* actionDynamicsGraph = nullptr;

    // Обновляет содержимое монет
    void displayEachAsset();
    // Обновляет содержимое кошелька
    void displayMyWallet();

private slots:
    void onClickedButtonOk();
    void onDialogTableActivated();
    void onDynamicsGraphActivated();

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

    StatusPanel(QWidget* parent = nullptr);

    TableController* getTable() {
        return table.get();
    }

signals:
    void clicked_btn();
    void triggered_table();
    void triggered_graph();
};
