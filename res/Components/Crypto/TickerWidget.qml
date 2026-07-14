import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import MainApplication 1.0
import Theme 1.0
import Components.Custom 1.0

Rectangle {
    id: root
    Layout.fillWidth: true

    color: "transparent"

    implicitHeight: contentLayout.implicitHeight + (contentLayout.anchors.margins * 2)
    implicitWidth: contentLayout.implicitWidth + (contentLayout.anchors.margins * 2)

    readonly property ticker pTicker: AppCore.ticker

    // Свойства для каждого значения
    QtObject {
        id: privateData

        // Свойства для измения содержимого элемента при наведении (true когда наведено)
        property bool showPriceInUSD: hoverPrice.hovered
        property bool showChangeAbsolute: hoverChange.hovered
        property bool showVolumeRaw: hoverVolume.hovered
    }

    // Главный контент тикера
    RowLayout {
        id: contentLayout
        anchors.fill: parent
        TickerItem {
            Layout.preferredWidth: 100
            titleItem: privateData.showPriceInUSD ? "Price, USD" : "Price"
            contentItem: privateData.showPriceInUSD ? pTicker.usdIndexPrice.toFixed(2) : pTicker.lastPrice.toFixed(2)

            HoverHandler { id: hoverPrice }
        } // Price

        TickerItem {
            Layout.preferredWidth: 120
            titleItem: privateData.showChangeAbsolute ? "Change, 24h" : "Change, 24h, %"
            contentItem: privateData.showChangeAbsolute ? (pTicker.lastPrice - pTicker.prevPrice24h).toFixed(2) : pTicker.price24hPcnt.toFixed(2)

            HoverHandler { id: hoverChange }

        } // Change

        TickerItem {
            Layout.preferredWidth: 100
            titleItem: "Max"
            contentItem: pTicker.high24h.toFixed(2)
        } // Max

        TickerItem {
            Layout.preferredWidth: 100
            titleItem: "Min"
            contentItem: pTicker.low24h.toFixed(2)
        } // Min

        TickerItem {
            Layout.preferredWidth: 130
            titleItem: privateData.showVolumeRaw ? "Volume, 24h" : "Volume (Ccy), 24h"
            contentItem: privateData.showVolumeRaw ? pTicker.vol24h.toFixed(2) : pTicker.volCcy24h.toFixed(2)

            HoverHandler { id: hoverVolume }
        } // Volume

        Item { Layout.fillWidth: true }

    } // contentLayout

} // TickerWidget