import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import Engine 1.0
import Components.Crypto 1.0
import Components.Custom 1.0
import Theme 1.0

Item {
    id: root

    Layout.fillWidth: true
    Layout.fillHeight: true

    property real orderbookWidth: 300

    property bool orderbookVisible: true

    //onOrderbookVisibleChanged:

    function setOrderbookVisible(visible) {
        orderbookPanel.visible = visible
    }

    function bindWithEngine() {
        tickerWidget.bindTicker()
        orderbookWidget.bindOrderbook()
    }

    function unbindWithEngine() {
        tickerWidget.unbindTicker()
        orderbookWidget.unbindOrderbook()
    }

    Item {
        id: leftContentContainer
        anchors.fill: parent

        ColumnLayout {
            id: mainContent
            anchors.fill: parent
            spacing: -1

            Rectangle {
                Layout.alignment: Qt.AlignTop
                Layout.fillWidth: true
                color: Theme.windowColor
                border.width: 1
                border.color: Theme.borderColor
                clip: true

                implicitHeight: Math.max(selecterTrade.implicitHeight, tickerWidget.implicitHeight)

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 1
                    spacing: Theme.spacing

                    SelecterTradeWidget {
                        id: selecterTrade
                        Layout.fillWidth: true
                        Layout.preferredWidth: parent.width / 6

                        popupList: Engine.tradeList

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
                }
            } // Rectangle

            RowLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true

                KlineWidget {
                    Layout.margins: Theme.margins
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    currentTrade: selecterTrade.currentTrade
                } // Candle chart

                Rectangle {
                    id: orderbookPanel
                    color: Theme.windowColor
                    Layout.fillHeight: true
                    Layout.preferredWidth: root.orderbookVisible ? orderbookWidth : 0
                    visible: root.orderbookVisible
                    clip: true
                    border.width: 1
                    border.color: Theme.borderColor

                    onVisibleChanged: {
                        root.orderbookWidth = visible ? width : 0
                    }

                    Behavior on Layout.preferredWidth {
                        NumberAnimation { duration: 200 }
                    }

                    OrderbookWidget {
                        id: orderbookWidget
                        anchors.fill: parent
                        anchors.margins: Theme.margins
                    } // orderbookWidget

                    CustomButton {
                        id: hideButton
                        text: ">"

                        implicitWidth: 20
                        implicitHeight: 20

                        anchors.left: parent.left
                        anchors.top: parent.top
                        anchors.leftMargin: 4
                        anchors.topMargin: 4
                        z: 2

                        contentItem: Text {
                            text: hideButton.text
                            font.pixelSize: 12
                            color: Theme.textColor
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }

                        background: Rectangle {
                            color: hideButton.hovered ? Theme.borderColor : "transparent"
                            border.width: 1
                            border.color: Theme.borderColor
                            radius: 2
                        }

                        onClicked: {
                            root.setOrderbookVisible(false)
                        }
                    } // hideButton
                } // orderbookPanel
            } // RowLayout

        } // mainContent
    } // leftContentContainer


    Component.onCompleted: {
        root.unbindWithEngine()
        root.bindWithEngine()
        root.setOrderbookVisible(true)
    }

    Component.onDestruction: root.unbindWithEngine()

} // TradePage
