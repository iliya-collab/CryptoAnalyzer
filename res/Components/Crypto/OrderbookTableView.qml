import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import MainApplication 1.0
import Theme 1.0

Item {
    id: root

    readonly property OrderbookSideModel asks: AppCore.asks
    readonly property OrderbookSideModel bids: AppCore.bids
    readonly property real maxVolume: Math.max(asks.maxVolume, bids.maxVolume)

    Connections {
        target: asks
        function onCountChanged() { orderbookCanvas.requestPaint() }
        function onDataChanged() { orderbookCanvas.requestPaint() }
    }

    Connections {
        target: bids
        function onCountChanged() { orderbookCanvas.requestPaint() }
        function onDataChanged() { orderbookCanvas.requestPaint() }
    }

    Row {
        id: rowHeader
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 10
        height: 30

        Text {
            width: parent.width * 0.5
            text: "BIDS"
            color: "#00ff66"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        Text {
            width: parent.width * 0.5
            text: "ASKS"
            color: "#ff4444"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    } // rowHeader

    Canvas {
        id: orderbookCanvas
        anchors.top: rowHeader.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        property int hoveredBidIndex: -1
        property int hoveredAskIndex: -1

        readonly property int leftMargin: Theme.margins
        readonly property int rightMargin: Theme.margins
        readonly property int topMargin: Theme.margins
        readonly property int bottomMargin: Theme.margins
        readonly property int chartWidth: width - leftMargin - rightMargin
        readonly property int chartHeight: height - topMargin - bottomMargin
        readonly property double halfWidth: chartWidth / 2
        readonly property int numberOfLevels: asks.count
        readonly property double rowHeight: chartHeight / numberOfLevels

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onPositionChanged: function(mouse) {
                var mx = mouse.x
                var my = mouse.y

                // Выход за пределы orderbookCanvas
                if (mx < orderbookCanvas.leftMargin || mx > orderbookCanvas.leftMargin + orderbookCanvas.chartWidth ||
                    my < orderbookCanvas.topMargin || my > orderbookCanvas.topMargin + orderbookCanvas.chartHeight) {
                    orderbookCanvas.hoveredAskIndex = -1
                    orderbookCanvas.hoveredBidIndex = -1
                    orderbookCanvas.requestPaint()
                    return
                }

                // Определяем строну и индекс уровня под курсором
                if (mx < orderbookCanvas.leftMargin + orderbookCanvas.halfWidth) {
                    // Сторона Bids
                    if (orderbookCanvas.numberOfLevels === 0)
                        orderbookCanvas.hoveredBidIndex = -1
                    else {
                        var idx = Math.floor((my - orderbookCanvas.topMargin) / orderbookCanvas.rowHeight)
                        if (idx >= 0 && idx < orderbookCanvas.numberOfLevels) {
                            orderbookCanvas.hoveredBidIndex = idx
                            orderbookCanvas.hoveredAskIndex = -1
                        }
                        else
                            orderbookCanvas.hoveredBidIndex = -1
                    }
                }
                else {
                    // Сторона Asks
                    if (orderbookCanvas.numberOfLevels === 0)
                        orderbookCanvas.hoveredAskIndex = -1
                    else {
                        var idx = Math.floor((my - orderbookCanvas.topMargin) / orderbookCanvas.rowHeight)
                        if (idx >= 0 && idx < orderbookCanvas.numberOfLevels) {
                            orderbookCanvas.hoveredBidIndex = -1
                            orderbookCanvas.hoveredAskIndex = idx
                        }
                        else
                            orderbookCanvas.hoveredAskIndex = -1
                    }
                }

                orderbookCanvas.requestPaint()
            }
            onExited: {
                orderbookCanvas.hoveredAskIndex = -1
                orderbookCanvas.hoveredBidIndex = -1
                orderbookCanvas.requestPaint()
            }
        }

        onPaint: {
            var ctx = getContext("2d")
            ctx.clearRect(0, 0, width, height)

            // Вертикальная разделительная линия
            ctx.strokeStyle = "#4a4a6a"
            ctx.lineWidth = 1
            ctx.beginPath()
            ctx.moveTo(leftMargin + halfWidth, topMargin)
            ctx.lineTo(leftMargin + halfWidth, topMargin + chartHeight)
            ctx.stroke()

            ctx.font = "11px monospace"
            ctx.textBaseline = "middle"

            // Рисуем Bids
            if (orderbookCanvas.numberOfLevels > 0) {
                for (var j = 0; j < orderbookCanvas.numberOfLevels; j++) {
                    var bid = bids.get(j)
                    var volPercentBid = maxVolume > 0 ? (bid.volume / maxVolume) : 0
                    var barWidthBid = volPercentBid * halfWidth
                    var xBid = leftMargin + halfWidth - barWidthBid
                    var yBid = topMargin + j * rowHeight

                    // Градиент
                    var grad = ctx.createLinearGradient(xBid, yBid, leftMargin + halfWidth, yBid)
                    if (j <= orderbookCanvas.hoveredBidIndex) {
                        grad.addColorStop(0, "rgba(76, 175, 80, 0.1)")
                        grad.addColorStop(1, "rgba(76, 175, 80, 0.8)")
                    } else {
                        grad.addColorStop(0, "rgba(76, 175, 80, 0.05)")
                        grad.addColorStop(1, "rgba(76, 175, 80, 0.3)")
                    }
                    ctx.fillStyle = grad
                    ctx.fillRect(xBid, yBid, barWidthBid, rowHeight - 1)

                    // Текст цены
                    ctx.fillStyle = "#8bc34a"
                    ctx.textAlign = "right"
                    ctx.fillText(bid.price.toFixed(2), leftMargin + halfWidth - 5, yBid + rowHeight / 2)

                    // Текст обьема
                    ctx.fillStyle = "#ffffff"
                    ctx.textAlign = "left"
                    ctx.fillText(bid.volume.toFixed(2), leftMargin + 5, yBid + rowHeight / 2)
                }
            }

            // Рисуем Asks
            if (orderbookCanvas.numberOfLevels > 0) {
                for (var k = 0; k < orderbookCanvas.numberOfLevels; k++) {
                    var ask = asks.get(k)
                    var volPercentAsk = maxVolume > 0 ? (ask.volume / maxVolume) : 0
                    var barWidthAsk = volPercentAsk * halfWidth
                    var xAsk = leftMargin + halfWidth
                    var yAsk = topMargin + k * rowHeight

                    // Градиент фона уровня
                    var gradAsk = ctx.createLinearGradient(xAsk, yAsk, xAsk + barWidthAsk, yAsk)
                    if (k <= orderbookCanvas.hoveredAskIndex) {
                        gradAsk.addColorStop(0, "rgba(244, 67, 54, 0.8)")
                        gradAsk.addColorStop(1, "rgba(244, 67, 54, 0.1)")
                    } else {
                        gradAsk.addColorStop(0, "rgba(244, 67, 54, 0.3)")
                        gradAsk.addColorStop(1, "rgba(244, 67, 54, 0.05)")
                    }
                    ctx.fillStyle = gradAsk
                    ctx.fillRect(xAsk, yAsk, barWidthAsk, rowHeight - 1)

                    // Текст цены
                    ctx.fillStyle = "#ef5350"
                    ctx.textAlign = "left"
                    ctx.fillText(ask.price.toFixed(2), leftMargin + halfWidth + 5, yAsk + rowHeight / 2)

                    // Текст объема
                    ctx.fillStyle = "#ffffff"
                    ctx.textAlign = "right"
                    ctx.fillText(ask.volume.toFixed(2), leftMargin + chartWidth - 5, yAsk + rowHeight / 2)
                }
            }

            // Выделеный диапозон
            if (orderbookCanvas.hoveredBidIndex >= 0 && orderbookCanvas.numberOfLevels > 0) {
                var lastBidIdx = orderbookCanvas.hoveredBidIndex
                var totalHeightBid = (lastBidIdx + 1) * rowHeight
                ctx.strokeStyle = "#8bc34a"
                ctx.lineWidth = 1
                ctx.strokeRect(leftMargin, topMargin, halfWidth, totalHeightBid)
                ctx.fillStyle = "rgba(139, 195, 74, 0.08)"
                ctx.fillRect(leftMargin, topMargin, halfWidth, totalHeightBid)
            } else if (orderbookCanvas.hoveredAskIndex >= 0 && orderbookCanvas.numberOfLevels > 0) {
                var lastAskIdx = orderbookCanvas.hoveredAskIndex
                var totalHeightAsk = (lastAskIdx + 1) * rowHeight
                ctx.strokeStyle = "#ef5350"
                ctx.lineWidth = 1
                ctx.strokeRect(leftMargin + halfWidth, topMargin, halfWidth, totalHeightAsk)
                ctx.fillStyle = "rgba(244, 67, 54, 0.08)"
                ctx.fillRect(leftMargin + halfWidth, topMargin, halfWidth, totalHeightAsk)
            }
        } // onPaint

        // Обновление при изменении моделей
        onWidthChanged: requestPaint()

        onHeightChanged: requestPaint()

    } // orderbookCanvas
} // root

