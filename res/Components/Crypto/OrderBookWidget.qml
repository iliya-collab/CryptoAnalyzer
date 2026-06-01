import QtQuick
import QtQuick.Layouts

import Engine 1.0
import Theme 1.0
import Components.Custom 1.0
import EngineComponents 1.0

Rectangle {
    id: root

    function updateOrderBook(newOrderBook) {
        orderBookTable.update(newOrderBook.bids, newOrderBook.asks)
        //console.log(newOrderBook.bids)
    }

    ColumnLayout {
        id: contentLayout
        anchors.fill: parent

        OrderBookTable {
            id: orderBookTable
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }

    Component.onCompleted: Engine.orderBookUpdated.connect(updateOrderBook)

    Component.onDestruction: Engine.orderBookUpdated.disconnect(updateOrderBook)

}
