#include "CustomWidgets/SpotWidget.hpp"

SpotWidget::SpotWidget(QWidget* parent) : QWidget(parent) {
    setupWidget();
}

void SpotWidget::updateListCoins(const QStringList& lst) {
    QStandardItemModel* currentModel = qobject_cast<QStandardItemModel*>(listCoins->model());

    if (!currentModel)
        return;

    currentModel->clear();
    for (const QString& coin : lst) {
        QStandardItem* item = new QStandardItem(coin);
        currentModel->appendRow(item);
    }
}

void SpotWidget::selectedItem(const QModelIndex &index) {
    QString selectedCoin = index.data().toString();
    emit selected(selectedCoin);
}

void SpotWidget::setupWidget() {
    QStandardItemModel* model = new QStandardItemModel(this);

    listCoins = new QListView(this);
    listCoins->setViewMode(QListView::ListMode);

    listCoins->setModel(model);

    listCoins->setSelectionMode(QAbstractItemView::SingleSelection);
    listCoins->setEditTriggers(QAbstractItemView::NoEditTriggers);

    listCoins->setMinimumHeight(150);
    listCoins->setMaximumHeight(300);
    listCoins->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    connect(listCoins, &QListView::clicked, this, &SpotWidget::selectedItem);

    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(listCoins);
}