#pragma once

#include "CustomWindowDialogs/IDialog.hpp"
#include "CustomWindowDialogs/DialogTable.hpp"
#include "CustomWindowDialogs/DialogGraph.hpp"
#include "CustomWindowDialogs/DialogOrderBook.hpp"

#include "CustomWidgets/TreeViewWidget.hpp"

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

class DialogScanner : public IDialog {
    Q_OBJECT
private:

    std::unique_ptr<Scanner> _scan;

    void setupUI() override;
    void connectionSignals();
    void setupMenu();

    QPushButton* btnOK;
    QPushButton* btnAdd;
    QPushButton* btnDel;

    TreeViewWidget* treeViewer;

    QComboBox* comboStockMarket;
    QComboBox* comboMarket;
    QComboBox* comboChannel;

    //std::unique_ptr<TableController> cntlTable;
    QAction* actionTabel = nullptr;
    //std::unique_ptr<DDynamicsGraph> DGraph;
    QAction* actionGraph = nullptr;

    //std::unique_ptr<OrderBookController> cntlOrderBook;
    QAction* actionOrderBooks = nullptr;

    QVBoxLayout* mainLayout;

private slots:

    void updateTable(const WebSocketParser::stTicker& _ticker);
    void updateGraph(const WebSocketParser::stTicker& _ticker);

    void updateOrderBook(const WebSocketParser::stOrderBooks& _orderBooks);

    void onClickedButtonOk();
    void onClickedButtonAdd();
    void onClickedButtonDel();

    void onDialogTableActivated();
    void onDialogGraphActivated();

    void onDialogOrderBookActivated();

public:

    DialogScanner(QWidget* parent = nullptr);

    Scanner* scanner();
    
};
