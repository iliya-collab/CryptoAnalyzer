#include "CustomWindowDialogs/DialogTable.hpp"

#include <QHeaderView>
#include <QTableWidgetItem>
#include <memory>

DialogTable::DialogTable(QWidget* parent) : IDialog(parent) {
    setAttribute(Qt::WA_DeleteOnClose);
    setModal(false);
    setWindowTitle("Table");
    setMinimumSize(300, 200);
    resize(500, 400);

    setupUI();
}

void DialogTable::setupUI() {
    tableWidget = new QTableWidget(this);
    tableWidget->setShowGrid(true);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(tableWidget);
}

void DialogTable::setSize(qint64 cols, qint64 rows) {
    if (cols > 0)
        colTable = cols;
    if (rows > 0)
        rowTable = rows;
    tableWidget->setRowCount(rowTable);
    tableWidget->setColumnCount(colTable);
}

void DialogTable::setHorizontalHeader(const QStringList& headers) {
    horHeaders = headers;
    tableWidget->horizontalHeader()->setVisible(true);
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->setHorizontalHeaderLabels(headers);
}

void DialogTable::setVerticalHeader(const QStringList& headers) {
    verHeaders = headers;
    tableWidget->verticalHeader()->setVisible(true);
    tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->setVerticalHeaderLabels(headers);
}

void DialogTable::fillTable(qint64 iRow, qint64 iCol, const QString& data) {
    tableWidget->setItem(iRow, iCol, new QTableWidgetItem(data));
    tableWidget->viewport()->update();
}

qint64 DialogTable::findIndexByHorizontalHeaders(const QString& str) {
    return horHeaders.indexOf(str);
}

qint64 DialogTable::findIndexByVerticalHeaders(const QString& str) {
    return verHeaders.indexOf(str);
}

/*TableController::TableController(QWidget* parent) {
    connect(_table.get(), &QDialog::finished, this, [this]() { 
        tableIsOpen = false;
        _table.release();
        _table = nullptr; 
    });
}

DialogTable* TableController::table() {
    return _table.get();
}

void TableController::showTable(qint64 cols, qint64 rows, const QStringList& verHeaders, const QStringList& horHeaders) {
    tableIsOpen = true;
    _table = std::make_unique<DialogTable>();
    _table->setSize(cols, rows);
    _table->setVerticalHeader(verHeaders);
    _table->setHorizontalHeader(horHeaders);
    _table->showDialog();
}

void TableController::updateTable(const QString &coin, double price) {
    if (!tableIsOpen)
        return;

    if (_table == nullptr)
        

    QStringList lst = coin.split(':'); // lst[0] - market , lst[1] - coin
    qint64 col = _table->findIndexByHorizontalHeaders(lst[0]);
    qint64 row = _table->findIndexByVerticalHeaders(lst[1]);
    _table->fillTable(row, col, QString::number(price, 'f', 3));
}*/
