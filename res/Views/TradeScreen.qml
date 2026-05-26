import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import Engine 1.0
import Components 1.0
import Theme 1.0

Rectangle {
    id: root

    Layout.fillWidth: true
    Layout.fillHeight: true

    color: Theme.windowColor

    RowLayout {
        Layout.alignment: Qt.AlignTop
        Layout.fillWidth: true

        implicitHeight: Math.max(selecterTrade.implicitHeight, tickerWidget.implicitHeight)

        SelecterTradeWidget {
            id: selecterTrade
            Layout.fillWidth: true
            Layout.fillHeight: true

            popupList: Engine.tradeList
            //popupWidth: 200
            //popupHeight: 200

            onItemSelected: function(item) {
                Engine.addTrade(item)
                Engine.filter(item)
            }
            onFilterSelected: function(filter) { Engine.loadTradingPairs(filter) }
        }

        TickerWidget {
            id: tickerWidget
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

    } // RowLayout

    OrderBookWidget {

    }

    Item { Layout.fillHeight: true }

} // TradePage
