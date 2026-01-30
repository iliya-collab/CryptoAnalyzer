#pragma once

#include "CustomWindowDialogs/IDialog.hpp"
#include "CustomWindowDialogs/TableController.hpp"
#include "CustomWindowDialogs/ViewerOrderBooksController.hpp"
#include "CustomWindowDialogs/DialogGraph.hpp"
#include "CustomWidgets/TreeViewWidget.hpp"
#include "Parser/Scanner.hpp"

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QCheckBox>
#include <QMap>
#include <QMenuBar>
#include <QMenu>
#include <QAction>

class DialogScanner : public IDialog {
    Q_OBJECT
private:

    void setupUI() override;
    void connectionSignals();
    void setupMenu();

    QWidget* createOrderBooksWidget();

    std::unique_ptr<Scanner> m_scanner;

    QVBoxLayout* mainLayout;

    QPushButton* btnStart;
    QPushButton* btnStop;
    QPushButton* btnAdd;
    QPushButton* btnDel;
    TreeViewWidget* treeViewer;
    QComboBox* comboStockMarket;

    QCheckBox* tickerChannelCheck;
    QButtonGroup* booksGroup;
    QRadioButton* booksNoneRadio;
    QRadioButton* books5Radio;
    QRadioButton* books10Radio;
    QRadioButton* books20Radio;

    std::unique_ptr<TableController> m_crtl_table;
    std::unique_ptr<ViewerOrderBooksController> m_crtl_ord_books;
    
    QSet<QString> Channels;

    QAction* actionChannels;
    QAction* actionTabel;
    QAction* actionGraph;
    QAction* actionTicker;
    QAction* actionOrderBooks;

private slots:

    void onClickedButtonAdd();
    void onClickedButtonDel();

    void onCurrentTextChanged(const QString& text);

public:

    DialogScanner(QWidget* parent = nullptr);

    Scanner* scanner();
    
};
