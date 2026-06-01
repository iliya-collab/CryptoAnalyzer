#pragma once

#include <QAbstractListModel>

struct OrderBookLevel {
    double price;
    double amount;
    bool isBid;
};

class OrderBookModel : public QAbstractListModel {
    Q_OBJECT
public:

    enum OrderBookRoles {
        PriceRole = Qt::UserRole + 1,
        AmountRole,
        TypeRole
    };

    explicit OrderBookModel(QObject *parent = nullptr);

    // Обязательные методы QAbstractListModel
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void updateFromQml(const QVariantList &bids, const QVariantList &asks);

private:

    QVector<OrderBookLevel> m_records;

};
