#include "PositionModel.hpp"

namespace Core::Tools
{

    int PositionModel::rowCount(const QModelIndex& parent) const
    {
        if (parent.isValid())
            return 0;
        return m_positions.size();
    }

    QVariant PositionModel::data(const QModelIndex& index, int role) const
    {
        if (!index.isValid() || index.row() >= m_positions.size())
            return {};

        const PositionInfo& p = m_positions.at(index.row());

        switch (role)
        {
        case CategoryRole:
            return QVariant::fromValue(p.m_category);
        case SymbolRole:
            return p.m_symbol;
        case SideRole:
            return QVariant::fromValue(p.m_side);
        case SizeRole:
            return p.m_size;
        case PositionIdxRole:
            return p.m_positionIdx;
        case AvgPriceRole:
            return p.m_avgPrice;
        case LiqPriceRole:
            return p.m_liqPrice;
        case MarkPriceRole:
            return p.m_markPrice;
        case LeverageRole:
            return p.m_leverage;
        case PositionIMRole:
            return p.m_positionIM;
        case PositionMMRole:
            return p.m_positionMM;
        case UnrealisedPnlRole:
            return p.m_unrealisedPnl;
        case CumRealisedPnlRole:
            return p.m_cumRealisedPnl;
        case AdlRankIndicatorRole:
            return p.m_adlRankIndicator;
        default:
            return {};
        }
    }

    QHash<int, QByteArray> PositionModel::roleNames() const
    {
        return {
            { CategoryRole,         "category" },
            { SymbolRole,           "symbol" },
            { SideRole,             "side" },
            { SizeRole,             "size" },
            { PositionIdxRole,      "positionIdx" },
            { AvgPriceRole,         "avgPrice" },
            { LiqPriceRole,         "liqPrice" },
            { MarkPriceRole,        "markPrice" },
            { LeverageRole,         "leverage" },
            { PositionIMRole,       "positionIM" },
            { PositionMMRole,       "positionMM" },
            { UnrealisedPnlRole,    "unrealisedPnl" },
            { CumRealisedPnlRole,   "cumRealisedPnl" },
            { AdlRankIndicatorRole, "adlRankIndicator" },
        };
    }

    QString PositionModel::hashKey(const PositionKey& key)
    {
        return key.symbol + QLatin1Char('|') + QString::number(key.positionIdx);
    }

    void PositionModel::upsertPosition(const PositionInfo& position)
    {
        const QString key = hashKey(keyOf(position));
        auto it = m_keyToRow.find(key);

        // Позиция закрыта (size == 0) — убираем строку, если она была
        if (position.m_size == 0.0)
        {
            if (it != m_keyToRow.end())
                removeAt(it.value());
            return;
        }

        if (it != m_keyToRow.end())
        {
            const int row = it.value();
            m_positions[row] = position;
            const QModelIndex idx = index(row);
            emit dataChanged(idx, idx);
            return;
        }

        const int row = m_positions.size();
        beginInsertRows(QModelIndex(), row, row);
        m_positions.append(position);
        m_keyToRow.insert(key, row);
        endInsertRows();
    }

    void PositionModel::resetPositions(const QList<PositionInfo>& positions)
    {
        beginResetModel();
        m_positions.clear();
        for (const auto& p : positions)
            if (p.m_size != 0.0)
                m_positions.append(p);
        rebuildIndex();
        endResetModel();
    }

    PositionInfo PositionModel::get(int row) const
    {
        if (row < 0 || row >= m_positions.size())
            return {};
        return m_positions.at(row);
    }

    PositionInfo PositionModel::findBySymbol(const QString& symbol, int positionIdx) const
    {
        const QString key = hashKey({ symbol, positionIdx });
        auto it = m_keyToRow.find(key);
        if (it == m_keyToRow.end())
            return {};
        return m_positions.at(it.value());
    }

    void PositionModel::removeAt(int row)
    {
        beginRemoveRows(QModelIndex(), row, row);
        m_positions.removeAt(row);
        endRemoveRows();
        rebuildIndex();
    }

    void PositionModel::rebuildIndex()
    {
        m_keyToRow.clear();
        m_keyToRow.reserve(m_positions.size());
        for (int i = 0; i < m_positions.size(); ++i)
            m_keyToRow.insert(hashKey(keyOf(m_positions.at(i))), i);
    }

}