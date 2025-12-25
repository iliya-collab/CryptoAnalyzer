#ifndef _DIALOG_TABEL_CURRENCY_RATES_
#define _DIALOG_TABEL_CURRENCY_RATES_

#include "CustomQDialog.hpp"
#include "../Parser/ParserCB.hpp"

#include <QTableWidget>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QLabel>
#include <QLineEdit>

class DTableCurrencyRates : public CustomQDialog {
    Q_OBJECT
public:
    DTableCurrencyRates(int cols, int rows, const QString& _date, const QList<CurrencyRateCB>& ListCurrencyRates, QWidget* parent = nullptr);
    ~DTableCurrencyRates();

private:
    
    void setupUI() override;
    void connectionSignals() override;
    void fillTable(const QList<CurrencyRateCB>& ListCurrencyRates);

    QString date = "";
    int colTable = 0;
    int rowTable = 0;

    QString SearchCharCode;

    QLineEdit* lineSearch;
    QTableWidget* tableWidget;


private slots:
    void onEditingFinished();
};

#endif