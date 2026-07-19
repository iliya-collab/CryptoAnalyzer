#pragma once
#include <QAbstractListModel>
#include <QtQml>

namespace Core::Tools {

    class OrderbookSideModel : public QAbstractListModel {
        Q_OBJECT
        QML_ELEMENT

        Q_PROPERTY(int count MEMBER m_count NOTIFY countChanged FINAL)
        Q_PROPERTY(Side side READ getSide WRITE setSide NOTIFY sideChanged FINAL)
        Q_PROPERTY(double maxVolume MEMBER m_maxVolume NOTIFY maxVolumeChanged FINAL)
        Q_PROPERTY(double totalVolume MEMBER m_totalVolume NOTIFY totalVolumeChanged FINAL)
        Q_PROPERTY(double totalTurnover MEMBER m_totalTurnover NOTIFY totalTurnoverChanged FINAL)

    public:

        enum Side {
            Bid,
            Ask
        };
        Q_ENUM(Side)

        enum Roles {
            PriceRole = Qt::UserRole + 1,
            VolumeRole,
            TurnoverRole,
            TotalVolumeRole,
            TotalTurnoverRole,
            AvgPriceRole
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
        void maxVolumeChanged();
        void totalVolumeChanged();
        void totalTurnoverChanged();

    private:

        struct Level {
            double price = 0;           // Цена на уровне
            double volume = 0;          // Обьем на уровне
            double turnover = 0;        // Оборот на уровне

            double totalVolume = 0;     // Весь обьем до n уровня
            double totalTurnover = 0;   // Весь Оборот до n уровня
            double avgPrice = 0;        // Средняя цена за n уровней

            bool operator!=(const Level& other) const {
                return  this->price != other.price ||
                        this->volume != other.volume ||
                        this->turnover != other.turnover;
            }

            bool operator==(const Level& other) const {
                return  this->price == other.price &&
                        this->volume == other.volume &&
                        this->turnover == other.turnover;
            }

        };

        int m_count = 0;
        double m_maxVolume = 0;
        double m_totalVolume = 0;
        double m_totalTurnover = 0;
        Side m_side;
        QVector<Level> m_levels;

    };

}
