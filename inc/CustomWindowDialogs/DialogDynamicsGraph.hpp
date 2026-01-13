#pragma once

#include "CustomQDialog.hpp"

#include <QLabel>
#include <QLineEdit>
#include <QtCharts>
#include <QList>
#include <QPushButton>

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

    void initParsers();
    void createGraph();
    void setupUI() override;
    void connectionSignals() override;

private slots:
    void onClickedButtonUpdate();

};
