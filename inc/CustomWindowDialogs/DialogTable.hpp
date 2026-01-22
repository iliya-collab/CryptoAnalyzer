#pragma once

#include "CustomWindowDialogs/IDialog.hpp"

#include <QVBoxLayout>
#include <QTableWidget>
#include <memory>

class DialogTable : public IDialog {
    Q_OBJECT
public:

    DialogTable(QWidget* parent = nullptr);
    ~DialogTable() = default;

    void setHorizontalHeader(const QStringList& headers);
    void setVerticalHeader(const QStringList& headers);
    void setSize(qint64 cols, qint64 rows);

    void fillTable(qint64 iRow, qint64 iCol, const QString& data);

    qint64 findIndexByHorizontalHeaders(const QString& str);
    qint64 findIndexByVerticalHeaders(const QString& str);

protected:

    void setupUI() override;

    QVBoxLayout* mainLayout;
    QTableWidget* tableWidget;

    qint64 colTable = 0;
    qint64 rowTable = 0;

    QStringList horHeaders;
    QStringList verHeaders;

};
