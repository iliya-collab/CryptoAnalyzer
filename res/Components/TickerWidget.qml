import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import Engine 1.0
import Theme 1.0

Rectangle {
    id: root
    Layout.fillWidth: true

    color: "transparent"

    implicitHeight: contentLayout.implicitHeight + (contentLayout.anchors.margins * 2)
    implicitWidth: contentLayout.implicitWidth + (contentLayout.anchors.margins * 2)

    // Свойства для каждого значения
    QtObject {
        id: privateData

        property string priceValue: ""
        property string priceValueInUSD: ""
        property string change24h: ""
        property string change24hPcnt: ""
        property string maxValue: ""
        property string minValue: ""
        property string volume24h: ""
        property string volumeCcy24h: ""

        // Свойства для измения содержимого элемента при наведении (true когда наведено)
        property bool showPriceInUSD: hoverPrice.hovered
        property bool showChangeAbsolute: hoverChange.hovered
        property bool showVolumeRaw: hoverVolume.hovered
    }

    // Функция для обновления данных тикера
    function updateTicker(newTicker) {
        privateData.priceValue = newTicker.lastPrice.toFixed(2)
        privateData.priceValueInUSD = newTicker.usdIndexPrice.toFixed(2)
        privateData.change24h = newTicker.prevPrice24h.toFixed(2)
        privateData.change24hPcnt = newTicker.price24hPcnt.toFixed(2)
        privateData.maxValue = newTicker.high24h.toFixed(2)
        privateData.minValue = newTicker.low24h.toFixed(2)
        privateData.volume24h = newTicker.vol24h.toFixed(2)
        privateData.volumeCcy24h = newTicker.volCcy24h.toFixed(2)
    }

    Component.onCompleted: Engine.tickerUpdated.connect(updateTicker)

    Component.onDestruction: Engine.tickerUpdated.disconnect(updateTicker)

    // Главный контент тикера
    RowLayout {
        id: contentLayout
        anchors.fill: parent
        TickerItem {
            titleItem: privateData.showPriceInUSD ? "Price, USD" : "Price"
            contentItem: privateData.showPriceInUSD ? privateData.priceValueInUSD : privateData.priceValue

            HoverHandler { id: hoverPrice }
        } // Price

        TickerItem {
            titleItem: privateData.showChangeAbsolute ? "Change, 24h" : "Change, 24h, %"
            contentItem: privateData.showChangeAbsolute ? privateData.change24h : privateData.change24hPcnt

            HoverHandler { id: hoverChange }

        } // Change

        TickerItem {
            titleItem: "Max"
            contentItem: privateData.maxValue
        } // Max

        TickerItem {
            titleItem: "Min"
            contentItem: privateData.minValue
        } // Min

        TickerItem {
            titleItem: privateData.showVolumeRaw ? "Volume, 24h" : "Volume (Ccy), 24h"
            contentItem: privateData.showVolumeRaw ? privateData.volume24h : privateData.volumeCcy24h

            HoverHandler { id: hoverVolume }
        } // Volume

        Item { Layout.fillWidth: true }

    } // contentLayout
} // TickerWidget