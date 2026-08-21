#pragma once
#include <QSortFilterProxyModel>
#include <qqmlregistration.h>

namespace Core::Tools {

    class TradePairsFilterProxyModel : public QSortFilterProxyModel {
        Q_OBJECT
        QML_ELEMENT

        Q_PROPERTY(QString quoteCoinFilter READ getQuoteCoinFilter WRITE setQuoteCoinFilter NOTIFY quoteCoinFilterChanged FINAL)

    public:

        explicit TradePairsFilterProxyModel(QObject* parent = nullptr);

        QString getQuoteCoinFilter() const { return m_quoteCoinFilter; }
        void setQuoteCoinFilter(const QString &text);

    signals:
        void quoteCoinFilterChanged();

    protected:
        bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

    private:

        QString m_quoteCoinFilter = "";

    };

}


