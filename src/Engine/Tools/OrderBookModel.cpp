#include "Engine/Tools/OrderBookModel.hpp"

OrderBookModel::OrderBookModel(QObject *parent) : QAbstractListModel(parent) {}

int OrderBookModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;
    return m_records.size();
}

QVariant OrderBookModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= m_records.size())
        return QVariant();

    const auto &level = m_records.at(index.row());

    switch (role) {
        case PriceRole:  return level.price;
        case AmountRole: return level.amount;
        case TypeRole:   return level.isBid ? "bid" : "ask";
        default:         return QVariant();
    }
}

QHash<int, QByteArray> OrderBookModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[PriceRole]  = "price";
    roles[AmountRole] = "amount";
    roles[TypeRole]   = "type";
    return roles;
}

void OrderBookModel::updateFromQml(const QVariantList &bids, const QVariantList &asks) {
    std::vector<OrderBookLevel> new_records;
    new_records.reserve(asks.size() + bids.size());

    for (const auto &val : asks) {
        QVariantList pair = val.toList();
        if (pair.size() >= 2)
            new_records.push_back({pair.at(0).toDouble(), pair.at(1).toDouble(), false});
    }
    for (const auto &val : bids) {
        QVariantList pair = val.toList();
        if (pair.size() >= 2)
            new_records.push_back({pair.at(0).toDouble(), pair.at(1).toDouble(), true});
    }

    int old_size = m_records.size();
    int new_size = static_cast<int>(new_records.size());

    if (new_size < old_size) {
        beginRemoveRows(QModelIndex(), new_size, old_size - 1);
        m_records.resize(new_size);
        endRemoveRows();
    } else if (new_size > old_size) {
        beginInsertRows(QModelIndex(), old_size, new_size - 1);
        m_records.resize(new_size);
        endInsertRows();
    }

    int min_size = std::min(old_size, new_size);
    int first_changed_row = -1;
    int last_changed_row = -1;

    for (int i = 0; i < new_size; ++i) {
        const auto &new_item = new_records[i];

        if (i >= min_size ||
            m_records[i].price != new_item.price ||
            m_records[i].amount != new_item.amount ||
            m_records[i].isBid != new_item.isBid)
        {
            m_records[i] = {new_item.price, new_item.amount, new_item.isBid};

            if (first_changed_row == -1)
                first_changed_row = i;
            last_changed_row = i;
        }
    }

    if (first_changed_row != -1) {
        QModelIndex topLeft = index(first_changed_row, 0);
        QModelIndex bottomRight = index(last_changed_row, 0);
        emit dataChanged(topLeft, bottomRight);
    }
}