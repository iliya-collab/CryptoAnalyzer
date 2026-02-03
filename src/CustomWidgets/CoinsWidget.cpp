#include "CustomWidgets/CoinsWidget.hpp"

CoinsWidget::CoinsWidget(QWidget* parent) : CustomWidget(parent) {
    setupWidget();
}

void CoinsWidget::setList(const QStringList& _lst) {
    model->clear();
    for (const QString& coin : _lst) {
        QStandardItem* item = new QStandardItem(coin);
        model->appendRow(item);
    }
}

void CoinsWidget::setupWidget() {
    model = new QStandardItemModel(m_widget);

    listCoins = new QListView(m_widget);
    listCoins->setModel(model);

    listCoins->setSelectionMode(QAbstractItemView::SingleSelection);
    listCoins->setEditTriggers(QAbstractItemView::NoEditTriggers);

    listCoins->setMinimumHeight(150);
    listCoins->setMaximumHeight(300);
    listCoins->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    connect(listCoins, &QListView::clicked, this, [this](const QModelIndex &index){
        QString selectedCoin = index.data().toString();
        qDebug() << "Selected:" << selectedCoin;
    });

    listCoins->setStyleSheet(R"(
        QListView {
            border: 1px solid #4a4a4a;
            border-radius: 4px;
            outline: none;
            font-size: 10px;
        }
        QListView::item {
            color: #ffffff;
            padding: 8px;
        }
        QListView::item:selected {
            background-color: #4a6fa5;
            color: white;
        }
    )");

    mainLayout = new QVBoxLayout(m_widget);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(listCoins);
}