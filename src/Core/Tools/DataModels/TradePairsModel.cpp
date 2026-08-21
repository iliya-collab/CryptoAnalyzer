#include "TradePairsModel.hpp"

namespace Core::Tools {

    int TradePairsModel::rowCount(const QModelIndex &parent) const {
        return m_pairs.size();
    }

    QVariant TradePairsModel::data(const QModelIndex &index, int role) const {

        if (!index.isValid() || index.row() >= m_pairs.size())
            return QVariant();

        const auto& trade = m_pairs.at(index.row());

        switch (role) {
        case SymbolRole:
            return trade.symbol;
        case QuoteRole:
            return trade.quote_coin;
        case BaseRole:
            return trade.base_coin;
        default:
            return QVariant();
        };

    }

    QHash<int, QByteArray> TradePairsModel::roleNames() const {
        return {
            {SymbolRole, "symbol"},
            {QuoteRole, "quote"},
            {BaseRole, "base"}
        };
    }

    void TradePairsModel::updateTradePairs(const QList<TradeInfo>& pairs) {
        beginResetModel();
        m_pairs = pairs;
        endResetModel();
    }

}
