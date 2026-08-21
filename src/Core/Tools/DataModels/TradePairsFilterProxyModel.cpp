#include "TradePairsFilterProxyModel.hpp"
#include "TradePairsModel.hpp"

namespace Core::Tools {

    TradePairsFilterProxyModel::TradePairsFilterProxyModel(QObject *parent)
        : QSortFilterProxyModel(parent)
    {
        setDynamicSortFilter(true);
        setFilterCaseSensitivity(Qt::CaseInsensitive);
    }

    void TradePairsFilterProxyModel::setQuoteCoinFilter(const QString &text)
    {
        if (m_quoteCoinFilter == text)
            return;

        m_quoteCoinFilter = text;

        emit quoteCoinFilterChanged();

        invalidate();
    }

    bool TradePairsFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
        if (m_quoteCoinFilter.isEmpty())
            return true;

        auto idx = sourceModel()->index(sourceRow, 0, sourceParent);

        QString quoteCoin = sourceModel()->data(idx, TradePairsModel::QuoteRole).toString();

        bool accepted = (quoteCoin == m_quoteCoinFilter);
        qDebug() << "filterAcceptsRow:" << sourceRow
                 << "quoteCoin=" << quoteCoin
                 << "filter=" << m_quoteCoinFilter
                 << "accepted=" << accepted;
        return accepted;;
    }

}

