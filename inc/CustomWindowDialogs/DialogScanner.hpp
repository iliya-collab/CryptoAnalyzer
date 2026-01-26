#pragma once

#include "CustomWindowDialogs/IDialog.hpp"
#include "CustomWindowDialogs/TableController.hpp"
#include "CustomWindowDialogs/DialogGraph.hpp"

#include "CustomWindowDialogs/ViewerOrderBooksController.hpp"

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

    std::unique_ptr<TableController> m_crtl_table;
    QAction* actionTabel = nullptr;
    QAction* actionGraph = nullptr;

    QAction* actionTicker = nullptr;
    std::unique_ptr<ViewerOrderBooksController> m_crtl_ord_books;
    QAction* actionOrderBooks = nullptr;

    QVBoxLayout* mainLayout;

private slots:

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
