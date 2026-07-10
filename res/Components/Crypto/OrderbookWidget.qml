import QtQuick
import QtQuick.Layouts
import Engine 1.0
import Theme 1.0
import Components.Custom 1.0

Item {
    id: root

    property double totalVolumeOrderbook: orderbook.bids.total + orderbook.asks.total
    property double percentAsk: totalVolumeOrderbook > 0 ? Math.round(orderbook.asks.total / totalVolumeOrderbook * 100) : 50
    property double percentBid: totalVolumeOrderbook > 0 ? Math.round(orderbook.bids.total / totalVolumeOrderbook * 100) : 50

    function updateOrderbook(newOrderbook) {
        orderbook.bids.update(newOrderbook.bids)
        orderbook.asks.update(newOrderbook.asks)
    }

    function bindOrderbook() {
        Engine.orderbookUpdated.connect(root.updateOrderbook)
    }

    function unbindOrderbook() {
        Engine.orderbookUpdated.disconnect(root.updateOrderbook)
    }

    ColumnLayout {
        id: contentLayout
        anchors.fill: parent
        spacing: 0

        OrderbookTableView {
            id: orderbook
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.margins: Theme.margins
        }

        IndicatorMarketSentiment {
            Layout.fillWidth: true
            Layout.margins: Theme.margins
            Layout.preferredHeight: 20

            percentAsk: root.percentAsk
            percentBid: root.percentBid
        }
    }

}
