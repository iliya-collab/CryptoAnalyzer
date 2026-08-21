import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Application.Core 1.0
import Components.Crypto 1.0
import Components.Custom 1.0
import Theme 1.0

Item {
    id: root

    Layout.fillWidth: true
    Layout.fillHeight: true

    property real rightPanel: 500
    property alias enableVolumeChart: candleChart.enableVolumeChart

    function showRecentTrades() {
        stackWidgets.replace("qrc:/qt/qml/Components/Crypto/TradeWidget.qml")
    }

    function showOrderbook() {
        stackWidgets.replace("qrc:/qt/qml/Components/Crypto/OrderbookWidget.qml")
    }

    Component.onCompleted: console.log("TradeScreen ready")

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
                        Layout.preferredWidth: 100


                        onItemSelected: function(item) {
                            AppCore.marketService.subscribeSymbol(item)
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
                    id: candleChart
                    Layout.margins: Theme.margins
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    currentTrade: selecterTrade.currentTrade

                    Component.onCompleted: console.log("KlineWidget ready")
                }

                Rectangle {
                    color: Theme.windowColor
                    Layout.fillHeight: true
                    Layout.preferredWidth: root.rightPanel
                    clip: true
                    border.width: 1
                    border.color: Theme.borderColor

                    StackView {
                        id: stackWidgets
                        anchors.fill: parent
                        anchors.margins: Theme.margins

                        initialItem: OrderbookWidget {}
                    } // stackWidgets

                } // Rectangle
            } // RowLayout

        } // mainContent
    } // leftContentContainer
} // TradePage
