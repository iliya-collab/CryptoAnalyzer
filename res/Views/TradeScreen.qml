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

    function bindWithEngine() {
        tickerWidget.bindTicker()
        orderbookWidget.bindOrderbook()
        klineWidget.bindKline()
    }

    function unbindWithEngine() {
        tickerWidget.unbindTicker()
        orderbookWidget.unbindOrderbook()
        klineWidget.unbindKline()
    }

    Item {
        id: leftContentContainer
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: rightPanel.visible ? rightPanel.left : parent.right

        ColumnLayout {
            id: mainContent
            anchors.fill: parent
            spacing: Theme.spacing

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
                    }
                    onFilterSelected: function(filter) {
                        Engine.loadTradesFromRepository(filter)
                    }
                }
                TickerWidget {
                    id: tickerWidget
                    Layout.fillWidth: true
                }
            } // RowLayout

            KlineWidget {
                id: klineWidget
                Layout.margins: Theme.margins
                Layout.fillHeight: true
                Layout.fillWidth: true
            }

        } // mainContent
    } // leftContentContainer

    Rectangle {
        id: rightPanel
        height: root.height
        width: root.width / 3

        color: Theme.windowColor

        anchors.right: parent.right
        anchors.top: parent.top

        onVisibleChanged: {
            root.rightPanelWidth = visible ? width : 0
        }

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: Theme.margins
            OrderbookWidget {
                id: orderbookWidget
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }
    } // rightPanel

    Component.onCompleted: {
        root.unbindWithEngine()
        root.bindWithEngine()
        root.setOrderbookVisible(true)
    }

    Component.onDestruction: root.unbindWithEngine()

} // TradePage
