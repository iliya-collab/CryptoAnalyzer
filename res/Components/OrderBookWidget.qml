import QtQuick

import Engine 1.0
import Theme 1.0

Rectangle {
    id: root

    // Свойства для каждого значения
    QtObject {
        id: privateData

        property var modelAsks: []
        property var modelBids: []
    }

    // Функция для обновления данных стакана заявак
    function updateOrderBook(newOrderBook) {
        privateData.modelAsks = newOrderBook.asks
        privateData.modelBids = newOrderBook.bids
    }

    Component.onCompleted: Engine.orderBookUpdated.connect(updateOrderBook)

    Component.onDestruction: Engine.orderBookUpdated.disconnect(updateOrderBook)

}
