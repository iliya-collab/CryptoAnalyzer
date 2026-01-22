#pragma once

#include "CustomWindowDialogs/IDialog.hpp"

#include <QtCharts>

class DialogGraph : public IDialog {
    Q_OBJECT
public:

    DialogGraph(QWidget* parent = nullptr);
    ~DialogGraph() = default;


private:

    void setupUI() override;

    QVBoxLayout* mainLayout;
    QChartView* chartView;
};
