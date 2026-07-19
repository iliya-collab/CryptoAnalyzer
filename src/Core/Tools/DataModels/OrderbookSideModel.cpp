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
            case PriceRole:
                return level.price;
            case VolumeRole:
                return level.volume;
            case TurnoverRole:
                return level.turnover;
            case TotalVolumeRole:
                return level.totalVolume;
            case TotalTurnoverRole:
                return level.totalTurnover;
            case AvgPriceRole:
                return level.avgPrice;
            default:
                return QVariant();
        }
    }

    QHash<int, QByteArray> OrderbookSideModel::roleNames() const {
        QHash<int, QByteArray> roles;

        roles[PriceRole]  = "price";
        roles[VolumeRole] = "volume";
        roles[TurnoverRole] = "turnover";
        roles[TotalVolumeRole] = "totalVolume";
        roles[TotalTurnoverRole] = "totalTurnover";
        roles[AvgPriceRole] = "avgPrice";

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

        double cumulativeVolume = 0.0;
        double cumulativeTurnover = 0.0;
        double cumulativePrice = 0.0;
        double maxVol = 0.0;
        int nLevel = 1;
        for (auto& level : new_levels) {
            level.turnover = level.price * level.volume;

            cumulativePrice += level.price;
            cumulativeVolume += level.volume;
            cumulativeTurnover += level.turnover;

            level.totalVolume = cumulativeVolume;
            level.totalTurnover = cumulativeTurnover;
            level.avgPrice = cumulativePrice / nLevel;

            if (level.volume > maxVol)
                maxVol = level.volume;

            nLevel++;
        }
        m_maxVolume = maxVol;
        m_totalVolume = cumulativeVolume;
        m_totalTurnover = cumulativeTurnover;

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

        emit maxVolumeChanged();
        emit totalVolumeChanged();
        emit totalTurnoverChanged();
    }

    QVariantMap OrderbookSideModel::get(int index) const {
        QVariantMap res;

        if (index < 0 || index >= m_levels.count())
            return res;

        auto item = m_levels[index];

        res["price"] = item.price;
        res["volume"] = item.volume;
        res["turnover"] = item.turnover;
        res["totalVolume"] = item.totalVolume;
        res["totalTurnover"] = item.totalTurnover;
        res["avgPrice"] = item.avgPrice;

        return res;
    }

    void OrderbookSideModel::update(const QVariantList& data) {
        QVector<Level> new_levels;
        new_levels.reserve(data.size());

        for (const auto& lvl : data) {
            QVariantList pair = lvl.toList();
            if (pair.size() >= 2)
                new_levels.push_back({pair.at(0).toDouble(), pair.at(1).toDouble(), 0.0, 0.0});
        }

        if (m_side == Bid)
            std::sort(new_levels.begin(), new_levels.end(), [](const Level &a, const Level &b) {
                return a.price > b.price;
            });
        else
            std::sort(new_levels.begin(), new_levels.end(), [](const Level &a, const Level &b) {
                return a.price < b.price;
            });

        double cumulativeVolume = 0.0;
        double cumulativeTurnover = 0.0;
        double cumulativePrice = 0.0;
        double maxVol = 0.0;
        int nLevel = 1;
        for (auto& level : new_levels) {
            level.turnover = level.price * level.volume;

            cumulativeVolume += level.volume;
            cumulativeTurnover += level.turnover;
            cumulativePrice += level.price;

            level.totalVolume = cumulativeVolume;
            level.totalTurnover = cumulativeTurnover;
            level.avgPrice = cumulativePrice / nLevel;

            if (level.volume > maxVol)
                maxVol = level.volume;

            nLevel++;
        }
        m_maxVolume = maxVol;
        m_totalVolume = cumulativeVolume;
        m_totalTurnover = cumulativeTurnover;

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

        emit maxVolumeChanged();
        emit totalVolumeChanged();
        emit totalTurnoverChanged();
    }

    void OrderbookSideModel::clear() {
        beginResetModel();
        m_levels.clear();
        m_count = 0;
        m_maxVolume = 0;
        m_totalVolume = 0;
        m_totalTurnover = 0;
        endResetModel();
        emit countChanged();
        emit maxVolumeChanged();
        emit totalVolumeChanged();
        emit totalTurnoverChanged();
    }

}