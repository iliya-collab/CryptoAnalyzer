import QtQuick
import QtQuick.Layouts
import Theme 1.0
import Components.Custom 1.0

Item {
    id: root

    property double totalVolumeOrderbook: viewOrderbook.bids.total + viewOrderbook.asks.total
    property double percentAsk: totalVolumeOrderbook > 0 ? Math.round(viewOrderbook.asks.total / totalVolumeOrderbook * 100) : 50
    property double percentBid: totalVolumeOrderbook > 0 ? Math.round(viewOrderbook.bids.total / totalVolumeOrderbook * 100) : 50

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
