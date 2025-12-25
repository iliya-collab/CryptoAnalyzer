#ifndef _DIALOG_DYNAMICS_GRAPH_
#define _DIALOG_DYNAMICS_GRAPH_

#include "Parser/ParserCB.hpp"
#include "CustomQDialog.hpp"

#include <QLabel>
#include <QLineEdit>
#include <QtCharts>
#include <QList>
#include <QPushButton>

#include <algorithm>

class DDynamicsGraph : public CustomQDialog {
    Q_OBJECT
public:
    DDynamicsGraph(QWidget* parent = nullptr);
    ~DDynamicsGraph();

    
private:

    QLineEdit* editDate1;
    QLineEdit* editDate2;
    QLineEdit* editID;
    QChartView* chartView;
    QPushButton* btnUpdate;

    ParserCB* parser_cb;
    QList<CurrencyRateCB> ListCurrencyRates;

    void initParsers();
    void createGraph();
    void setupUI() override;
    void connectionSignals() override;

private slots:
    void onClickedButtonUpdate();

};

#endif