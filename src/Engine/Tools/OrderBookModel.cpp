#include "Engine/Tools/OrderBookModel.hpp"

OrderBookSideModel::OrderBookSideModel(QObject *parent)
    : QAbstractListModel(parent), m_side(Bid) {}

int OrderBookSideModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;
    return m_levels.size();
}

QVariant OrderBookSideModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= m_levels.size())
        return QVariant();

    const auto &level = m_levels.at(index.row());

    switch (role) {
        case PriceRole:  return level.price;
        case AmountRole: return level.amount;
        default:         return QVariant();
    }
}

QHash<int, QByteArray> OrderBookSideModel::roleNames() const {
    QHash<int, QByteArray> roles;

    roles[PriceRole]  = "price";
    roles[AmountRole] = "amount";
    roles[TotalAmountRole] = "total";

    return roles;
}

void OrderBookSideModel::setSide(Side side) {
    if (m_side == side)
        return;

    m_side = side;

    emit sideChanged();
}

void OrderBookSideModel::updateData(const QVariantList& data) {
    std::vector<Level> new_levels;
    new_levels.reserve(data.size());

    for (const auto& val : data) {
        QVariantList pair = val.toList();
        if (pair.size() >= 2)
            new_levels.push_back({pair.at(0).toDouble(), pair.at(1).toDouble()});
    }

    if (m_side == Bid)
        std::sort(new_levels.begin(), new_levels.end(), [](const Level &a, const Level &b) {
            return a.price > b.price;
        });
    else
        std::sort(new_levels.begin(), new_levels.end(), [](const Level &a, const Level &b) {
            return a.price < b.price;
        });

    if (!new_levels.empty()) {
        double total = 0;
        for (auto& lvl : new_levels) {
            total += lvl.amount;
            lvl.total += total;
        }
    }

    int new_size = static_cast<int>(new_levels.size());
    int old_size = static_cast<int>(m_levels.size());

    if (new_size < old_size) {
        beginRemoveRows(QModelIndex(), new_size, old_size - 1);
        m_levels.resize(new_size);
        endRemoveRows();
    } else if (new_size > old_size) {
        beginInsertRows(QModelIndex(), old_size, new_size - 1);
        m_levels.resize(new_size);
        endInsertRows();
    }

    bool data_changed = false;
    int first_changed = -1;
    int last_changed = -1;

    for (int i = 0; i < new_size; ++i) {
        if (i < old_size && m_levels[i] != new_levels[i]) {
            m_levels[i] = new_levels[i];
            data_changed = true;

            if (first_changed == -1)
                first_changed = i;
            last_changed = i;
        } else {
            m_levels[i] = new_levels[i];
            data_changed = true;

            if (first_changed == -1)
                first_changed = i;
            last_changed = i;
        }
    }

    if (data_changed && first_changed != -1)
        emit dataChanged(index(first_changed, 0), index(last_changed, 0));
}