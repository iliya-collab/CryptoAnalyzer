import QtQuick
import QtQuick.Layouts
import Theme 1.0
import Components.Custom 1.0

Item {
    id: root

    property real totalVolumeOrderbook: viewOrderbook.bids.totalVolume + viewOrderbook.asks.totalVolume
    property real percentAsk: totalVolumeOrderbook > 0 ? Math.round(viewOrderbook.asks.totalVolume / totalVolumeOrderbook * 100) : 50
    property real percentBid: totalVolumeOrderbook > 0 ? Math.round(viewOrderbook.bids.totalVolume / totalVolumeOrderbook * 100) : 50

    ColumnLayout {
        id: contentLayout
        anchors.fill: parent
        spacing: 0

        OrderbookTableView {
            id: viewOrderbook
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
