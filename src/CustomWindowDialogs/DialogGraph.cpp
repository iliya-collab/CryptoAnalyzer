#include "CustomWindowDialogs/DialogGraph.hpp"

DialogGraph::DialogGraph(QWidget* parent) : IDialog(parent) {
    setAttribute(Qt::WA_DeleteOnClose);
    setModal(false);
    setWindowTitle("DynamicsGraph");
    setMinimumSize(300, 200);
    resize(500, 400);
    move(100, 100);

    setupUI();
}

void DialogGraph::setupUI() {
    chartView = new QChartView();
    chartView->setRenderHint(QPainter::Antialiasing);

    mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(chartView);
}

/*void DialogGraph::createGraph() {

    QList<QLineSeries*> ListSeries;
    QDate baseDate = QDate::fromString(editDate1->text().replace('/', '.'), "dd.MM.yyyy");
    int xPoint = 0;
    QString lastID = "";

    for (auto i : ListCurrencyRates) {

        if (lastID != i.ID) {
            lastID = i.ID;
            ListSeries.append(new QLineSeries(this));
            ListSeries[ListSeries.size()-1]->setName(i.CharCode);
            //qDebug() << "Series" << lastID;
        }

        QDate currentDate = QDate::fromString(i.Date, "dd.MM.yyyy");
        xPoint = baseDate.daysTo(currentDate);
        ListSeries[ListSeries.size()-1]->append(xPoint, i.Value);
        //qDebug() << QString("   (%1;%2)").arg(xPoint).arg(i.Value);

    }

    for (auto series : ListSeries)
        qDebug() << "Series name:" << series->name();

    QChart* chart = new QChart();
    chart->setTitle("Currency Graph");
    chart->removeAllSeries();
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
    for (auto series : ListSeries)
        chart->addSeries(series);

    QValueAxis* axisX = new QValueAxis();
    axisX->setLabelFormat("%d");
    axisX->setTitleText("Date (Days)");
    axisX->setTickType(QValueAxis::TickType::TicksDynamic);
    axisX->setTickInterval(1);

    QValueAxis* axisY = new QValueAxis();
    axisY->setLabelFormat("%.2f");
    axisY->setTitleText("Value");
    axisY->setTickType(QValueAxis::TickType::TicksDynamic);
    axisY->setTickInterval(1);

    qreal globalMinY = std::numeric_limits<qreal>::max();
    qreal globalMaxY = std::numeric_limits<qreal>::min();
    int globalMinX = INT_MAX;
    int globalMaxX = INT_MIN;

    for (auto series : ListSeries) {
        auto points = series->points();
        for (const auto& point : points) {
            globalMinX = qMin(globalMinX, (int)point.x());
            globalMaxX = qMax(globalMaxX, (int)point.x());
            globalMinY = qMin(globalMinY, point.y());
            globalMaxY = qMax(globalMaxY, point.y());
        }
    }

    qreal yMargin = (globalMaxY - globalMinY) * 0.1;
    axisX->setRange(globalMinX, globalMaxX);
    axisY->setRange(globalMinY - yMargin, globalMaxY + yMargin);

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);

    for (auto series : ListSeries) {
        series->attachAxis(axisX);
        series->attachAxis(axisY);
    }

    chartView->setChart(chart);

    chartView->update();

}*/
