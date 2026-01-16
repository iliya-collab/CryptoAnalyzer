#pragma once

#include "CustomQDialog.hpp"

#include <QTableWidget>
#include <memory>

class DialogTable : public CustomQDialog {
    Q_OBJECT
public:

    DialogTable(QWidget* parent = nullptr);
    ~DialogTable() = default;

    void setHorizontalHeader(const QStringList& headers);
    void setVerticalHeader(const QStringList& headers);
    void fillTable(qint64 iRow, qint64 iCol, const QString& data);
    void setSize(qint64 cols, qint64 rows);

    qint64 findIndexByHorizontalHeaders(const QString& str);
    qint64 findIndexByVerticalHeaders(const QString& str);

protected:

    void setupUI() override;
    void connectionSignals() override;

    qint64 colTable = 0;
    qint64 rowTable = 0;

    QStringList horHeaders;
    QStringList verHeaders;

    QTableWidget* tableWidget;
};


class TableController : QObject {
    Q_OBJECT
public:

    void updateTable(const QString &coin, double price);
    void showTable(qint64 cols, qint64 rows, const QStringList& verHeaders, const QStringList& horHeaders);

    DialogTable* table();

    TableController(QWidget* parent = nullptr);

private:


    std::unique_ptr<DialogTable> _table;

    bool tableIsOpen = false;

};
