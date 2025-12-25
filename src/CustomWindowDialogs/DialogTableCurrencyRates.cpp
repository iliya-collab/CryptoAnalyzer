#include "CustomWindowDialogs/DialogTableCurrencyRates.hpp"

DTableCurrencyRates::DTableCurrencyRates(int cols, int rows, const QString& _date, const QList<CurrencyRateCB>& ListCurrencyRates, QWidget* parent) : CustomQDialog(parent) {
    setAttribute(Qt::WA_DeleteOnClose);
    setModal(false);
    setWindowTitle("TabelCurrencyRates");
    setMinimumSize(300, 200);
    resize(500, 400);
    move(100, 100);

    colTable = cols;
    rowTable = rows;
    date = _date;

    setupUI();
    connectionSignals();
    fillTable(ListCurrencyRates);

    show();
}

DTableCurrencyRates::~DTableCurrencyRates() {
    delete lineSearch;
    delete tableWidget;
}

void DTableCurrencyRates::setupUI() {
    tableWidget = new QTableWidget(this);
    tableWidget->setRowCount(rowTable);
    tableWidget->setColumnCount(colTable);
    tableWidget->setShowGrid(true);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QStringList headers;
    headers << "ID" << "Name" << "CharCode" << "Nominal" << "Value(RUB)";
    
    tableWidget->horizontalHeader()->setVisible(true);
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->setHorizontalHeaderLabels(headers);

    QLabel* label_date;
    label_date = new QLabel("Information from: " + date, this);

    lineSearch = new QLineEdit(this);

    layout = new QVBoxLayout(this);
    layout->addWidget(label_date);
    layout->addWidget(lineSearch);
    layout->addWidget(tableWidget);
    setLayout(layout);
}

void DTableCurrencyRates::connectionSignals() {
    QObject::connect(lineSearch, &QLineEdit::editingFinished, this, &DTableCurrencyRates::onEditingFinished);
}

void DTableCurrencyRates::fillTable(const QList<CurrencyRateCB>& ListCurrencyRates) {
    for (int row = 0; row < rowTable; row++) {
        tableWidget->setItem(row, 0, new QTableWidgetItem(ListCurrencyRates[row].ID));
        tableWidget->setItem(row, 1, new QTableWidgetItem(ListCurrencyRates[row].Name));
        tableWidget->setItem(row, 2, new QTableWidgetItem(ListCurrencyRates[row].CharCode));
        tableWidget->setItem(row, 3, new QTableWidgetItem(QString::number(ListCurrencyRates[row].Nominal)));
        tableWidget->setItem(row, 4, new QTableWidgetItem(QString::number(ListCurrencyRates[row].Value, 'f', 4)));
    }
    tableWidget->viewport()->update();
}

void DTableCurrencyRates::onEditingFinished() {
    SearchCharCode = lineSearch->text();
    qDebug() << "Search:" << SearchCharCode;

    bool is_search = false;
    QTableWidgetItem* item;
    for (int row = 0; row < rowTable; row++) {
        item = tableWidget->item(row, 2);
        if (item->text() == SearchCharCode) {
            tableWidget->setCurrentItem(item);
            tableWidget->scrollToItem(item);
            tableWidget->setFocus();
            qDebug() << "Item found";
            is_search = true;
            break;
        }
    }
    if (!is_search)
        qDebug() << "Item not found";
}