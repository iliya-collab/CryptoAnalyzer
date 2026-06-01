import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import Engine 1.0
import Components.Crypto 1.0
import Theme 1.0

Rectangle {
    id: root

    Layout.fillWidth: true
    Layout.fillHeight: true

    color: Theme.windowColor

    property real rightPanelWidth: 0

    property bool orderbookVisible: rightPanel.visible

    function setOrderbookVisible(visible) {
        rightPanel.visible = visible
    }

    Item {
        id: leftContentContainer
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: rightPanel.visible ? rightPanel.left : parent.right

        RowLayout {
            id: mainContent
            anchors.fill: parent

            RowLayout {
                Layout.alignment: Qt.AlignTop
                Layout.fillWidth: true

                implicitHeight: Math.max(selecterTrade.implicitHeight, tickerWidget.implicitHeight)

                SelecterTradeWidget {
                    id: selecterTrade
                    Layout.fillWidth: true

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
                }
            } // RowLayout
        } // mainContent
    } // leftContentContainer

    Rectangle {
        id: rightPanel
        height: root.height
        width: root.width / 4

        color: Theme.windowColor
        border.width: Theme.borderWidth
        border.color: Theme.borderColor
        radius: Theme.radius

        anchors.right: parent.right
        anchors.top: parent.top

        onVisibleChanged: {
            root.rightPanelWidth = visible ? width : 0
        }

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: Theme.margins
            OrderBookWidget {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }
    } // rightPanel

    Component.onCompleted: {
        root.setOrderbookVisible(true)
        Engine.loadTradingPairs("ALL")
        if (Engine.tradeList && Engine.tradeList.length > 0) {
            var firstItem = Engine.tradeList[0]
            Engine.addTrade(firstItem)
            Engine.filter(firstItem)
        }
    }

} // TradePage
