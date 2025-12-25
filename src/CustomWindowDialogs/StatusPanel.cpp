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

    outputResults = new QTextEdit(this);
    outputResults->setReadOnly(true);
    outputResults->setFocusPolicy(Qt::NoFocus);

    outputDynamic = new QTextEdit(this);
    outputDynamic->setReadOnly(true);
    outputDynamic->setFocusPolicy(Qt::NoFocus);

    QHBoxLayout* row = new QHBoxLayout;
    btnOK = new QPushButton(this);
    btnOK->setText("OK");;
    btnOK->setFocusPolicy(Qt::NoFocus);

    row->addWidget(btnOK);
    row->addStretch();
    row->addStretch();

    layout = new QVBoxLayout(this);
    layout->addWidget(outputDynamic);
    layout->addWidget(outputResults);
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

void StatusPanel::display() {
    // Сохраняем текущее положение прокрутки
    int scrollValue = outputDynamic->verticalScrollBar()->value();
    outputDynamic->clear();

    displayEachAsset();
    outputDynamic->append("");
    displayMyWallet();

    // Восстанавливаем положение прокрутки
    outputDynamic->verticalScrollBar()->setValue(scrollValue);
}

void StatusPanel::displayResults(double val) {
    static int nScan = 1;

    outputResults->clear();

    outputResults->append(QString("Sanning #%1\nRevenue : %2\nAll revenue : %3")
        .arg(nScan)
        .arg(QString::number(val, 'f', 4))
        .arg(QString::number(ParserMyWallet::getAllRevenue(), 'f', 4)));

    auto amounts = ParserMyWallet::getCurrentAmounts();

    for (auto [coin, amount] : amounts.asKeyValueRange()) {
        QTextCursor cursor = outputResults->textCursor();

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
    nScan++;
}

void StatusPanel::displayMyWallet() {

    auto statW = ParserMyWallet::getStat().statWallet;

    outputDynamic->append("Total price");
    QTextCursor cursor = outputDynamic->textCursor();

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

    outputDynamic->append("Asset changes");

    for (auto [coin, stat] : statA.asKeyValueRange()) {
        QTextCursor cursor = outputDynamic->textCursor();

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
