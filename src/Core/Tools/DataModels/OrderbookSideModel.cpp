#include "OrderbookSideModel.hpp"

namespace Core::Tools {

    OrderbookSideModel::OrderbookSideModel(QObject *parent)
        : QAbstractListModel(parent), m_side(Bid) {}

    OrderbookSideModel::OrderbookSideModel(Side side, QObject *parent)
        : QAbstractListModel(parent), m_side(side) {}

    int OrderbookSideModel::rowCount(const QModelIndex &parent) const {
        if (parent.isValid())
            return 0;
        return m_levels.size();
    }

    QVariant OrderbookSideModel::data(const QModelIndex &index, int role) const {
        if (!index.isValid() || index.row() >= m_levels.size())
            return QVariant();

        const auto &level = m_levels.at(index.row());

        switch (role) {
            case PriceRole:  return level.price;
            case VolumeRole: return level.volume;
            default:         return QVariant();
        }
    }

    QHash<int, QByteArray> OrderbookSideModel::roleNames() const {
        QHash<int, QByteArray> roles;

        roles[PriceRole]  = "price";
        roles[VolumeRole] = "volume";
        roles[TotalVolumeRole] = "total";

        return roles;
    }

    void OrderbookSideModel::setSide(Side side) {
        if (m_side == side)
            return;

        m_side = side;

        emit sideChanged();
    }

    void OrderbookSideModel::update(const QMap<double, double>& data) {
        QVector<Level> new_levels;
        new_levels.reserve(data.size());

        if (m_side == Ask) {
            for (auto it = data.constBegin(); it != data.constEnd(); ++it)
                new_levels.push_back({it.key(), it.value(), 0.0});
        } else {
            for (auto it = data.constEnd(); it != data.constBegin(); ) {
                --it;
                new_levels.push_back({it.key(), it.value(), 0.0});
            }
        }

        double cumulative = 0.0;
        double maxVol = 0.0;
        for (auto& level : new_levels) {
            cumulative += level.volume;
            level.total = cumulative;
            if (level.volume > maxVol)
                maxVol = level.volume;
        }
        m_total = cumulative;
        m_maxVolume = maxVol;

        int new_size = new_levels.size();
        int old_size = m_levels.size();

        if (new_size < old_size) {
            beginRemoveRows(QModelIndex(), new_size, old_size - 1);
            m_levels.resize(new_size);
            endRemoveRows();
        } else if (new_size > old_size) {
            beginInsertRows(QModelIndex(), old_size, new_size - 1);
            m_levels.resize(new_size);
            endInsertRows();
        }

        bool hasDataChanged = false;
        int firstChanged = -1;
        int lastChanged = -1;

        for (int i = 0; i < new_size; ++i) {
            if (i < old_size) {
                if (m_levels[i] != new_levels[i]) {
                    m_levels[i] = new_levels[i];
                    hasDataChanged = true;
                    if (firstChanged == -1) firstChanged = i;
                    lastChanged = i;
                }
            } else {
                m_levels[i] = new_levels[i];
                hasDataChanged = true;
                if (firstChanged == -1) firstChanged = i;
                lastChanged = i;
            }
        }

        if (m_count != m_levels.size()) {
            m_count = m_levels.size();
            emit countChanged();
        }

        if (hasDataChanged && firstChanged != -1)
            emit dataChanged(index(firstChanged, 0), index(lastChanged, 0));

        emit totalChanged();
        emit maxVolumeChanged();
    }

    QVariantMap OrderbookSideModel::get(int index) const {
        QVariantMap res;

        if (index < 0 || index >= m_levels.count())
            return res;

        auto item = m_levels[index];

        res["price"] = item.price;
        res["volume"] = item.volume;
        res["total"] = item.total;

        return res;
    }

    void OrderbookSideModel::update(const QVariantList& data) {
        QVector<Level> new_levels;
        new_levels.reserve(data.size());

        for (const auto& lvl : data) {
            QVariantList pair = lvl.toList();
            if (pair.size() >= 2)
                new_levels.push_back({pair.at(0).toDouble(), pair.at(1).toDouble(), 0.0});
        }

        if (m_side == Bid)
            std::sort(new_levels.begin(), new_levels.end(), [](const Level &a, const Level &b) {
                return a.price > b.price;
            });
        else
            std::sort(new_levels.begin(), new_levels.end(), [](const Level &a, const Level &b) {
                return a.price < b.price;
            });

        double cumulative = 0.0;
        double maxVol = 0.0;
        for (auto& level : new_levels) {
            cumulative += level.volume;
            level.total = cumulative;
            if (level.volume > maxVol)
                maxVol = level.volume;
        }
        m_total = cumulative;
        m_maxVolume = maxVol;

        int new_size = new_levels.size();
        int old_size = m_levels.size();

        if (new_size < old_size) {
            beginRemoveRows(QModelIndex(), new_size, old_size - 1);
            m_levels.resize(new_size);
            endRemoveRows();
        } else if (new_size > old_size) {
            beginInsertRows(QModelIndex(), old_size, new_size - 1);
            m_levels.resize(new_size);
            endInsertRows();
        }

        bool hasDataChanged = false;
        int firstChanged = -1;
        int lastChanged = -1;

        for (int i = 0; i < new_size; ++i) {
            if (i < old_size) {
                if (m_levels[i] != new_levels[i]) {
                    m_levels[i] = new_levels[i];
                    hasDataChanged = true;
                    if (firstChanged == -1) firstChanged = i;
                    lastChanged = i;
                }
            } else {
                m_levels[i] = new_levels[i];
                hasDataChanged = true;
                if (firstChanged == -1) firstChanged = i;
                lastChanged = i;
            }
        }

        if (m_count != m_levels.size()) {
            m_count = m_levels.size();
            emit countChanged();
        }

        if (hasDataChanged && firstChanged != -1)
            emit dataChanged(index(firstChanged, 0), index(lastChanged, 0));

        emit totalChanged();
        emit maxVolumeChanged();

        /*std::vector<Level> new_levels;
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
                total += lvl.volume;
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

        if (m_count != m_levels.count()) {
            m_count = m_levels.count();
            emit countChanged();
        }

        if (data_changed && first_changed != -1)
            emit dataChanged(index(first_changed, 0), index(last_changed, 0));

        m_total = m_levels.last().total;
        emit totalChanged();

        m_maxVolume = 0;
        for (auto i : m_levels)
            m_maxVolume = std::max(m_maxVolume, i.volume);
        emit maxVolumeChanged();*/
    }

    void OrderbookSideModel::clear() {
        beginResetModel();
        m_levels.clear();
        m_count = 0;
        m_total = 0;
        m_maxVolume = 0;
        endResetModel();
        emit countChanged();
        emit totalChanged();
        emit maxVolumeChanged();
    }

}