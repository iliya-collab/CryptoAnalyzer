import QtQuick
import QtQuick.Layouts

import Engine 1.0
import Theme 1.0
import Components.Custom 1.0
import Engine.Components 1.0

Rectangle {
    id: root

    color: Theme.windowColor

    OrderBookSideModel {
        id: bidsModel
        side: OrderBookSideModel.Bid
    }

    OrderBookSideModel {
        id: asksModel
        side: OrderBookSideModel.Ask
    }

    function updateOrderBook(newOrderBook) {
        bidsModel.updateData(newOrderBook.bids)
        asksModel.updateData(newOrderBook.asks)
    }

    function bindOrderbook() {
        Engine.orderBookUpdated.connect(root.updateOrderBook)
    }

    function unbindOrderbook() {
        Engine.orderBookUpdated.disconnect(root.updateOrderBook)
    }

    RowLayout {
        id: contentLayout
        anchors.fill: parent
        anchors.margins: Theme.margins

        // Колонка Bids
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: 1

            CustomText {
                text: "BIDS"
                font.bold: true
                color: "#00ff66"
                Layout.fillWidth: true
                horizontalAlignment: CustomText.AlignHCenter
                verticalAlignment: CustomText.AlignVCenter
            }

            OrderBookTableView {
                id: tblBids
                model: bidsModel
                priceColor: "#00ff66"
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }

        // Колонка Asks
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: 1

            CustomText {
                text: "ASKS"
                font.bold: true
                color: "#ff4444"
                Layout.fillWidth: true
                horizontalAlignment: CustomText.AlignHCenter
                verticalAlignment: CustomText.AlignVCenter
            }

            OrderBookTableView {
                id: tblAsks
                model: asksModel
                priceColor: "#ff4444"
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }
    }

    //Component.onCompleted: Engine.orderBookUpdated.connect(updateOrderBook)
}
