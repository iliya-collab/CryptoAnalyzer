#pragma once

#include "CustomWindowDialogs/IDialog.hpp"
#include "Parser/WebSocketParser.hpp"

#include <QComboBox>
#include <QBoxLayout>
#include <QStackedWidget>
#include <QTableWidget>
#include <QList>
#include <QHash>
#include <QProgressBar>
#include <QHeaderView>
#include <QLabel>

class ViewerOrderBooks : public IDialog {
    Q_OBJECT
private:

    QComboBox* comboSubscribedCoins;
    QComboBox* comboStockMarkets;
    QStackedWidget* stackWidgets;
    QTableWidget* orderBooksTable;
    QLabel* lblSpread;
    QLabel* lblMessage;
    QVBoxLayout* mainLayout;

    void setupUI() override;

    void setupConnection();

    void setupOrderBooksTable();
    void setupMessage();

    void updateChannelAvailability(const QString& market);

    void updateSpread(double bestAsk, double bestBid);
    void updateAsks(const QList<WebSocketParser::Ask>& asks, double mxSize);
    void updateBids(const QList<WebSocketParser::Bid>& bids, double mxSize);

    QString m_stockMarket;
    QString m_pair;
    bool m_hasChannel;
    
private slots:
    
    void onComboTextChanged(const QString &text);

public:
    
    void setStockMarkets(const QStringList& lst);
    void setPairs(const QStringList& lst);
    
    QString getCurrentStockMarkets();
    QString getCurrentPair();
    
    void updateDisplay(const QList<WebSocketParser::Ask>& asks, const QList<WebSocketParser::Bid>& bids);

    explicit ViewerOrderBooks(QWidget* parent = nullptr);

};