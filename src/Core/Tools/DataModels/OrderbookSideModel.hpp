#pragma once
#include <QAbstractListModel>
#include <QtQml>

namespace Core::Tools {

    class OrderbookSideModel : public QAbstractListModel {
        Q_OBJECT
        QML_ELEMENT

        Q_PROPERTY(int count MEMBER m_count NOTIFY countChanged)
        Q_PROPERTY(Side side READ getSide WRITE setSide NOTIFY sideChanged)
        Q_PROPERTY(double total MEMBER m_total NOTIFY totalChanged)
        Q_PROPERTY(double maxVolume MEMBER m_maxVolume NOTIFY maxVolumeChanged)

    public:

        enum Side {
            Bid,
            Ask
        };
        Q_ENUM(Side)

        enum Roles {
            PriceRole = Qt::UserRole + 1,
            VolumeRole,
            TotalVolumeRole
        };

        explicit OrderbookSideModel(QObject* parent = nullptr);
        explicit OrderbookSideModel(Side side, QObject* parent = nullptr);

        // Обязательные методы QAbstractListModel
        int rowCount(const QModelIndex &parent = QModelIndex()) const override;
        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
        QHash<int, QByteArray> roleNames() const override;

        Side getSide() const { return m_side; }
        void setSide(Side side);
        void update(const QMap<double, double>& data);

        Q_INVOKABLE void update(const QVariantList& data);
        Q_INVOKABLE void clear();
        Q_INVOKABLE QVariantMap get(int index) const;

    signals:

        void countChanged();
        void sideChanged();
        void totalChanged();
        void maxVolumeChanged();

    private:

        struct Level {
            double price = 0;
            double volume = 0;
            double total = 0;

            bool operator!=(const Level& other) const {
                return  this->price != other.price ||
                        this->volume != other.volume;
            }

            bool operator==(const Level& other) const {
                return  this->price == other.price &&
                        this->volume == other.volume;
            }

        };

        int m_count = 0;
        double m_total = 0;
        double m_maxVolume = 0;
        Side m_side;
        QVector<Level> m_levels;

    };

}
