import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    Layout.fillWidth: true

    color: "transparent"
    visible: true
    border.width: 1
    border.color: sysPal.midlight

    implicitHeight: contentLayout.implicitHeight + (contentLayout.anchors.margins * 2)
    implicitWidth: contentLayout.implicitWidth + (contentLayout.anchors.margins * 2)

    // Свойства для каждого значения
    QtObject {
        id: privateData

        property string priceValue: ""
        property string change24h: ""
        property string change24hPcnt: ""
        property string maxValue: ""
        property string minValue: ""
        property string volume24h: ""
        property string volumeCcy24h: ""

        // Св-ва для измения содержимого элемента при наведении (true когда наведено)
        property bool showChangeAbsolute: hoverChange.hovered
        property bool showVolumeRaw: hoverVolume.hovered
    }

    // Функция для обновления данных тикера
    function updateTicker(ticker) {
        privateData.priceValue = ticker.lastPrice.toFixed(2)
        privateData.change24h = ticker.prevPrice24h.toFixed(2)
        privateData.change24hPcnt = (ticker.price24hPcnt * 100).toFixed(2)
        privateData.maxValue = ticker.high24h.toFixed(2)
        privateData.minValue = ticker.low24h.toFixed(2)
        privateData.volume24h = ticker.vol24h.toFixed(2)
        privateData.volumeCcy24h = ticker.volCcy24h.toFixed(2)
    }

    // Подключаемся к сигналу tickerUpdated из AppWorker
    Component.onCompleted: {
        // Если appEngine доступен в контексте
        if (typeof appEngine !== "undefined")
            appEngine.tickerUpdated.connect(updateTicker)
    }

    // Отключаемся от сигнала tickerUpdated из AppWorker
    Component.onDestruction: {
        if (typeof appEngine !== "undefined")
            appEngine.tickerUpdated.disconnect(updateTicker)
    }

    // Главный контент тикера
    RowLayout {
        id: contentLayout
        anchors.fill: parent
        anchors.margins: 5
        //spacing: 10

        TickerItem {
            titleItem: "Price"
            contentItem: privateData.priceValue
        } // Price

        TickerItem {
            titleItem: (privateData.showChangeAbsolute) ? "Change, 24h" : "Change, 24h, %"
            contentItem: (privateData.showChangeAbsolute) ? privateData.change24h : privateData.change24hPcnt

            // Обработка при наведении на поле
            HoverHandler {
                id: hoverChange
                onHoveredChanged: privateData.showChangeAbsolute = hovered
            }

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
            titleItem: (privateData.showVolumeRaw) ? "Volume, 24h" : "Volume (Ccy), 24h"
            contentItem: (privateData.showVolumeRaw) ? privateData.volume24h : privateData.volumeCcy24h

            // Обработка при наведении на поле
            HoverHandler {
                id: hoverVolume
                onHoveredChanged: privateData.showVolumeRaw = hovered
            }
        } // Volume

        Item { Layout.fillWidth: true }

    } // contentLayout
} // TickerWidget