import QtQuick
import QtQuick.Layouts

import Engine 1.0
import Theme 1.0
import Components.Custom 1.0
import Engine.Components 1.0

Rectangle {
    id: root

    color: Theme.windowColor

    OrderbookSideModel {
        id: bidsModel
        side: OrderbookSideModel.Bid
    }

    OrderbookSideModel {
        id: asksModel
        side: OrderbookSideModel.Ask
    }

    function updateOrderbook(newOrderbook) {
        bidsModel.updateData(newOrderbook.bids)
        asksModel.updateData(newOrderbook.asks)
    }

    function bindOrderbook() {
        Engine.orderbookUpdated.connect(root.updateOrderbook)
    }

    function unbindOrderbook() {
        Engine.orderbookUpdated.disconnect(root.updateOrderbook)
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

            OrderbookTableView {
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

            OrderbookTableView {
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
