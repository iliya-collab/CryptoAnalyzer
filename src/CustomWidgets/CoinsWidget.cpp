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

void CoinsWidget::selectedItem(const QModelIndex &index) {
    QString selectedCoin = index.data().toString();
    qDebug() << "Selected:" << selectedCoin;
    emit selected(selectedCoin);
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

    connect(listCoins, &QListView::clicked, this, &CoinsWidget::selectedItem);

    mainLayout = new QVBoxLayout(m_widget);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(listCoins);
}