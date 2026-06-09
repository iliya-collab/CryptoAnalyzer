#pragma once

#include <QAbstractListModel>

class OrderBookSideModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(Side side READ side WRITE setSide NOTIFY sideChanged)

public:

    enum Side {
        Bid,
        Ask
    };
    Q_ENUM(Side)

    enum Roles {
        PriceRole = Qt::UserRole + 1,
        AmountRole,
        TotalAmountRole
    };

    explicit OrderBookSideModel(QObject *parent = nullptr);

    // Обязательные методы QAbstractListModel
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Side side() const { return m_side; }
    void setSide(Side side);

    Q_INVOKABLE void updateData(const QVariantList &data);

signals:
    void sideChanged();

private:

    struct Level {
        double price = 0;
        double amount = 0;
        double total = 0;

        bool operator!=(const Level& other) const {
            return  this->price != other.price ||
                    this->amount != other.amount;
        }

        bool operator==(const Level& other) const {
            return  this->price == other.price &&
                    this->amount == other.amount;
        }

    };

    Side m_side;

    QVector<Level> m_levels;

};
