#pragma once

#include "CustomWindowDialogs/CustomQDialog.hpp"
#include "CustomWindowDialogs/DialogDynamicsGraph.hpp"
#include "CustomWindowDialogs/DialogTable.hpp"

#include "Parser/Scanner.hpp"

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QVBoxLayout>
#include <QMap>
#include <QMenuBar>
#include <QMenu>
#include <QAction>

class DialogScanner : public CustomQDialog {
    Q_OBJECT
private:

    std::unique_ptr<Scanner> _scan;

    void setupUI();
    void connectionSignals();
    void setupMenu();

    QPushButton* btnOK;
    QPushButton* btnAdd;

    QComboBox* comboStockMarket;
    QComboBox* comboMarket;
    QComboBox* comboChannel;

    std::unique_ptr<TableController> TController;
    QAction* actionTabel = nullptr;

    std::unique_ptr<DDynamicsGraph> DGraph;
    QAction* actionGraph = nullptr;

private slots:

    void updateTable(const QString &symbol, const WebSocketParser::stInfoCoin& _info);
    void updateGraph(const QString &symbol, const WebSocketParser::stInfoCoin& _info);

    void onClickedButtonOk();
    void onClickedButtonAdd();
    void onDialogTableActivated();
    void onDialogGraphActivated();

public:

    DialogScanner(QWidget* parent = nullptr);

    Scanner* scanner() {
        return _scan.get();
    }
    
};
