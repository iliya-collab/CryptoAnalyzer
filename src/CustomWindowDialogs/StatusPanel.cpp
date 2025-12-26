#include "CustomWindowDialogs/StatusPanel.hpp"

#include "Configs/MyWalletConfig.hpp"
#include "Parser/ParserMyWallet.hpp"

StatusPanel::StatusPanel(QWidget* parent) : CustomQDialog(parent) {
    resize(600, 400);
    setWindowTitle("Status panel");

    setupUI();
    connectionSignals();
}


void StatusPanel::setupUI() {

    outputStatWallet = new QTextEdit(this);
    outputStatWallet->setReadOnly(true);
    outputStatWallet->setFocusPolicy(Qt::NoFocus);

    outputStatAssets = new QTextEdit(this);
    outputStatAssets->setReadOnly(true);
    outputStatAssets->setFocusPolicy(Qt::NoFocus);

    outputMessages = new QTextEdit(this);
    outputMessages->setReadOnly(true);
    outputMessages->setFocusPolicy(Qt::NoFocus);

    QSplitter* verSplitter = new QSplitter(Qt::Vertical, this);
    QSplitter* horSplitter = new QSplitter(Qt::Horizontal, this);
    horSplitter->setFocusPolicy(Qt::NoFocus);
    verSplitter->setFocusPolicy(Qt::NoFocus);

    horSplitter->addWidget(outputStatAssets);
    horSplitter->addWidget(outputStatWallet);
    verSplitter->addWidget(horSplitter);
    verSplitter->addWidget(outputMessages);

    // позволяем разделителю управлять мышью
    horSplitter->setChildrenCollapsible(false);
    verSplitter->setChildrenCollapsible(false);

    outputStatAssets->setMinimumWidth(100);
    outputStatWallet->setMinimumWidth(100);
    outputMessages->setMinimumHeight(50);

    horSplitter->setSizes({300,400});
    verSplitter->setSizes({400,200});

    QHBoxLayout* row = new QHBoxLayout;
    btnOK = new QPushButton(this);
    btnOK->setText("OK");;
    btnOK->setFocusPolicy(Qt::NoFocus);

    row->addWidget(btnOK);
    row->addStretch();
    row->addStretch();

    layout = new QVBoxLayout(this);
    layout->addWidget(verSplitter);
    layout->addLayout(row);
}

void StatusPanel::connectionSignals() {
    connect(btnOK, &QPushButton::clicked, this, &StatusPanel::close);
}

void StatusPanel::showWidget() {
    QWidget::show();
}

void StatusPanel::show() {
    showWidget();
}

void StatusPanel::clearDisplay(StatusPanel::Display _disp) {
    switch (_disp)
    {
    case Display::StatAssets:
        outputStatAssets->clear();
        break;
    case Display::StatWallet:
        outputStatWallet->clear();
        break;
    case Display::Messages:
        outputMessages->clear();
        break;
    }
}

void StatusPanel::outputStringInDisplay(StatusPanel::Display _disp, const QString& str) {
    switch (_disp)
    {
    case Display::StatAssets:
        outputStatAssets->append(str);
        break;
    case Display::StatWallet:
        outputStatWallet->append(str);
        break;
    case Display::Messages:
        outputMessages->append(str);
        break;
    }
}

void StatusPanel::displayMessages(const QString& msg) {
    outputMessages->append(msg);
}

void StatusPanel::displayStatAssets() {
    // Сохраняем текущее положение прокрутки
    int scrollValue = outputStatAssets->verticalScrollBar()->value();

    displayEachAsset();
    outputStatAssets->append("");
    displayMyWallet();

    // Восстанавливаем положение прокрутки
    outputStatAssets->verticalScrollBar()->setValue(scrollValue);
}

void StatusPanel::displayStatWallet() {
    outputStatWallet->append(QString("All revenue : %1")
        .arg(QString::number(ParserMyWallet::getAllRevenue(), 'f', 4)));

    auto amounts = ParserMyWallet::getCurrentAmounts();

    for (auto [coin, amount] : amounts.asKeyValueRange()) {
        QTextCursor cursor = outputStatWallet->textCursor();

        // Создаем таблицу с 1 колонкой для группировки
        QTextTableFormat mainTableFormat;
        mainTableFormat.setBorder(0);
        mainTableFormat.setCellSpacing(0);
        mainTableFormat.setCellPadding(0);

        QTextTable *mainTable = cursor.insertTable(1, 1, mainTableFormat);

        // Теперь внутри этой ячейки создаем таблицу с 3 колонками
        QTextCursor innerCursor = mainTable->cellAt(0, 0).firstCursorPosition();

        QTextTableFormat innerTableFormat;
        innerTableFormat.setBorder(0);
        innerTableFormat.setCellSpacing(0);
        innerTableFormat.setCellPadding(5);

        QTextTable *innerTable = innerCursor.insertTable(1, 2, innerTableFormat);

        QTextCharFormat coinFormat, valueFormat;
        coinFormat.setForeground(QColor("white"));
        coinFormat.setFontWeight(QFont::Bold);
        valueFormat.setForeground(QColor("white"));

        QTextBlockFormat alignRight;
        alignRight.setAlignment(Qt::AlignRight);

        // Заполняем ячейки
        innerTable->cellAt(0, 0).firstCursorPosition().insertText(coin, coinFormat);

        innerTable->cellAt(0, 1).firstCursorPosition().insertText(QString::number(amounts[coin], 'f', 4), valueFormat);
    }
}

void StatusPanel::displayMyWallet() {
    auto statW = ParserMyWallet::getStat().statWallet;

    outputStatAssets->append("Total price");
    QTextCursor cursor = outputStatAssets->textCursor();

    // Создаем таблицу с 3 колонками
    QTextTableFormat tableFormat;
    tableFormat.setBorder(0); // Без границ
    tableFormat.setCellSpacing(0);
    tableFormat.setCellPadding(5);

    QTextTable *table = cursor.insertTable(1, 3, tableFormat);

    // Форматирование ячеек
    QTextCharFormat priceFormat, difFormat, perFormat;
    QString colorName = (statW.difTotalPrice == 0) ? "white" :
                        (statW.difTotalPrice > 0) ? "green" : "red";
    priceFormat.setForeground(QColor(colorName));
    difFormat.setForeground(QColor(colorName));
    perFormat.setForeground(QColor(colorName));

    // Выравнивание
    QTextBlockFormat alignRight;
    alignRight.setAlignment(Qt::AlignRight);

    // Заполнение ячеек
    QTextTableCell cell = table->cellAt(0, 0);
    QTextCursor cellCursor = cell.firstCursorPosition();
    cellCursor.insertText(QString::number(statW.totalPrice, 'f', 4), priceFormat);

    cell = table->cellAt(0, 1);
    cellCursor = cell.firstCursorPosition();
    cellCursor.setBlockFormat(alignRight);
    cellCursor.insertText(QString::number(statW.difTotalPrice, 'f', 4), difFormat);

    cell = table->cellAt(0, 2);
    cellCursor = cell.firstCursorPosition();
    cellCursor.setBlockFormat(alignRight);
    cellCursor.insertText(QString::number(statW.perDifPrice, 'f', 4) + "%", perFormat);
}

void StatusPanel::displayEachAsset() {
    auto statA = ParserMyWallet::getStat().statEachAsset;
    auto amount = ParserMyWallet::getCurrentAmounts();

    outputStatAssets->append("Asset changes");

    for (auto [coin, stat] : statA.asKeyValueRange()) {
        QTextCursor cursor = outputStatAssets->textCursor();

        // Создаем таблицу с 1 колонкой для группировки
        QTextTableFormat mainTableFormat;
        mainTableFormat.setBorder(0);
        mainTableFormat.setCellSpacing(0);
        mainTableFormat.setCellPadding(0);

        QTextTable *mainTable = cursor.insertTable(1, 1, mainTableFormat);

        // Теперь внутри этой ячейки создаем таблицу с 3 колонками
        QTextCursor innerCursor = mainTable->cellAt(0, 0).firstCursorPosition();

        QTextTableFormat innerTableFormat;
        innerTableFormat.setBorder(0);
        innerTableFormat.setCellSpacing(0);
        innerTableFormat.setCellPadding(5);

        QTextTable *innerTable = innerCursor.insertTable(1, 4, innerTableFormat);

        QString colorName = (stat.difTotalPrice == 0) ? "white" :
                            (stat.difTotalPrice > 0) ? "green" : "red";

        QTextCharFormat coinFormat, valueFormat;
        coinFormat.setForeground(QColor(colorName));
        coinFormat.setFontWeight(QFont::Bold);
        valueFormat.setForeground(QColor(colorName));

        QTextBlockFormat alignRight;
        alignRight.setAlignment(Qt::AlignRight);

        // Заполняем ячейки
        innerTable->cellAt(0, 0).firstCursorPosition().insertText(coin, coinFormat);

        innerTable->cellAt(0, 1).firstCursorPosition().insertText(QString::number(amount[coin], 'f', 4), valueFormat);

        QTextCursor priceCursor = innerTable->cellAt(0, 2).firstCursorPosition();
        priceCursor.setBlockFormat(alignRight);
        priceCursor.insertText(QString::number(stat.totalPrice, 'f', 4), valueFormat);

        QTextCursor changeCursor = innerTable->cellAt(0, 3).firstCursorPosition();
        changeCursor.setBlockFormat(alignRight);

        QString changeStr = QString("%1 (%2 : %3)")
            .arg(QString::number(stat.difTotalPrice, 'f', 4))
            .arg(QString::number(stat.perDifPrice, 'f', 4)+"%")
            .arg(QString::number(stat.perDependence, 'f', 4)+"%");

        changeCursor.insertText(changeStr, valueFormat);
    }
}
