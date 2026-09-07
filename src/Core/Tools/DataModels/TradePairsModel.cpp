#include "TradePairsModel.hpp"

namespace Core::Tools {

    int TradePairsModel::rowCount(const QModelIndex &parent) const
    {
        return m_pairs.size();
    }

    QVariant TradePairsModel::data(const QModelIndex &index, int role) const
    {

        if (!index.isValid() || index.row() >= m_pairs.size())
            return QVariant();

        const auto& trade = m_pairs.at(index.row());

        switch (role)
        {
        case SymbolRole:
            return trade.m_symbol;
        case QuoteRole:
            return trade.m_quoteCoin;
        case BaseRole:
            return trade.m_baseCoin;
        case CategoryRole:
            return QVariant::fromValue(trade.m_category);
        default:
            return QVariant();
        };

    }

    QHash<int, QByteArray> TradePairsModel::roleNames() const
    {
        return {
            {SymbolRole, "symbol"},
            {QuoteRole, "quote"},
            {BaseRole, "base"},
            {CategoryRole, "category"}
        };
    }

    void TradePairsModel::updateTradePairs(const QList<TradeInfo>& pairs)
    {
        beginResetModel();
        m_pairs = pairs;
        endResetModel();
    }

}
