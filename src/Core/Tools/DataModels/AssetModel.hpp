#pragma once
#include <QAbstractListModel>

namespace Core::Tools
{

    struct Asset {
        QString name;
        double amount;
        double value;
    };

    class AssetModel : public QAbstractListModel
    {
        Q_OBJECT

    public:

        enum AssetRoles {
            NameRole = Qt::UserRole + 1,
            AmountRole,
            ValueRole
        };

        explicit AssetModel(QObject* parent = nullptr) : QAbstractListModel(parent) {};

        int rowCount(const QModelIndex& parent = QModelIndex()) const override;
        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
        QHash<int, QByteArray> roleNames() const override;

        void updateAssets(std::vector<std::tuple<QString, double, double>> newAssets);

    private:

        std::vector<Asset> m_assets{};

    };

}


