#include "CustomWindowDialogs/DialogDynamicsGraph.hpp"

DDynamicsGraph::DDynamicsGraph(QWidget* parent) : CustomQDialog(parent) {
    setAttribute(Qt::WA_DeleteOnClose);
    setModal(false);
    setWindowTitle("DynamicsGraph");
    setMinimumSize(300, 200);
    resize(500, 400);
    move(100, 100);

    setupUI();
    connectionSignals();
    initParsers();

    show();
}

DDynamicsGraph::~DDynamicsGraph() {

}

void DDynamicsGraph::setupUI() {
    editDate1 = new QLineEdit(this);
    editDate2 = new QLineEdit(this);
    editID = new QLineEdit(this);
    btnUpdate = new QPushButton("Update", this);

    editDate1->setMaxLength(10);
    editDate1->setPlaceholderText("dd/mm/yyyy");
    editDate2->setMaxLength(10);
    editDate2->setPlaceholderText("dd/mm/yyyy");
    btnUpdate->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    btnUpdate->setFocusPolicy(Qt::NoFocus);
    editID->setPlaceholderText("ID");

    QHBoxLayout* row1 = new QHBoxLayout;
    row1->addWidget(editDate1);
    row1->addWidget(editDate2);
    row1->addWidget(editID);

    QHBoxLayout* row3 = new QHBoxLayout;
    row3->addWidget(btnUpdate);
    row3->addStretch();

    chartView = new QChartView();
    chartView->setRenderHint(QPainter::Antialiasing);
    
    layout = new QVBoxLayout(this);
    layout->addLayout(row1);
    layout->addWidget(chartView);
    layout->addLayout(row3);
}

void DDynamicsGraph::connectionSignals() {
    QObject::connect(btnUpdate, &QPushButton::clicked, this, &DDynamicsGraph::onClickedButtonUpdate);
}

void DDynamicsGraph::initParsers() {
    parser_cb = new ParserCB;
}

void DDynamicsGraph::createGraph() {

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

}

void DDynamicsGraph::onClickedButtonUpdate() {
    ListCurrencyRates.clear();
    if (editDate1->text() != "" || editDate2->text() != "" || editID->text() != "") {
        
        QStringList listID = editID->text().split(",");
        for (auto i : listID) {
            parser_cb->makeRequest(editDate1->text(), editDate2->text(), i);
            ListCurrencyRates += parser_cb->getCurrencyRates();

        }
    }
    createGraph();
}