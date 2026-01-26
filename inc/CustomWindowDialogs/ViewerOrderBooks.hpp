#pragma once

#include "CustomWindowDialogs/IDialog.hpp"
#include "Parser/WebSocketParser.hpp"

#include <QComboBox>
#include <QBoxLayout>
#include <QTableWidget>
#include <QList>
#include <QHash>
#include <QProgressBar>
#include <QHeaderView>

class ViewerOrderBooks : public IDialog {
    Q_OBJECT
private:

    QComboBox* subscribedCoins;
    QComboBox* usedStockMarkets;
    QTableWidget* orderBookTable;
    QVBoxLayout* mainLayout;

    void addHorSeparatorWidget(int row);
    void addVerSeparatorWidget(int row);

    void setupOrderBookTable();
    
    QString m_stockMarket;
    QString m_pair;
    
public:
    
    void setStockMarkets(const QStringList& lst);
    void setPairs(const QStringList& lst);
    
    QString getCurrentStockMarkets();
    QString getCurrentPair();
    
    void updateDisplay(const QList<WebSocketParser::Ask>& asks, const QList<WebSocketParser::Bid>& bids);

    explicit ViewerOrderBooks(QWidget* parent = nullptr);

    void setupUI() override;

};