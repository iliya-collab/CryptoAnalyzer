import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Engine.Components 1.0

Item {
    id: root

    readonly property alias asks: asksModel
    readonly property alias bids: bidsModel

    property real maxVolume: Math.max(bidsModel.maxVolume, asksModel.maxVolume)

    OrderbookSideModel {
        id: bidsModel
        side: OrderbookSideModel.Bid

        onCountChanged: orderbookCanvas.requestPaint()
        onDataChanged: orderbookCanvas.requestPaint()
    }

    OrderbookSideModel {
        id: asksModel
        side: OrderbookSideModel.Ask

        onCountChanged: orderbookCanvas.requestPaint()
        onDataChanged: orderbookCanvas.requestPaint()
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

        onPaint: {
            var ctx = getContext("2d")
            var w = width
            var h = height

            ctx.clearRect(0, 0, w, h)

            var leftMargin = 10
            var rightMargin = 10
            var topMargin = 10
            var bottomMargin = 10

            var chartWidth = w - leftMargin - rightMargin
            var chartHeight = h - topMargin - bottomMargin
            var halfWidth = chartWidth / 2

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
            var bidsCount = bidsModel.count
            if (bidsCount > 0) {
                var rowHeightBid = chartHeight / bidsCount

                for (var j = 0; j < bidsCount; j++) {
                    var bid = bidsModel.get(j)

                    var volPercentBid = maxVolume > 0 ? (bid.volume / maxVolume) : 0

                    var barWidthBid = volPercentBid * (halfWidth - 40)

                    var xBid = leftMargin + halfWidth - barWidthBid
                    var yBid = topMargin + j * rowHeightBid

                    // Градиент
                    var grad = ctx.createLinearGradient(xBid, yBid, leftMargin + halfWidth, yBid)
                    grad.addColorStop(0, "rgba(76, 175, 80, 0.05)")
                    grad.addColorStop(1, "rgba(76, 175, 80, 0.3)")

                    ctx.fillStyle = grad
                    ctx.fillRect(xBid, yBid, barWidthBid, rowHeightBid - 1)

                    // Текст цены
                    ctx.fillStyle = "#8bc34a"
                    ctx.textAlign = "right"
                    ctx.fillText(bid.price.toFixed(2), leftMargin + halfWidth - 5, yBid + rowHeightBid / 2)

                    // Текст обьема
                    ctx.fillStyle = "#ffffff"
                    ctx.textAlign = "left"
                    ctx.fillText(bid.volume.toFixed(2), leftMargin + 5, yBid + rowHeightBid / 2)
                }
            }

            // Рисуем Asks
            var asksCount = asksModel.count
            if (asksCount > 0) {
                var rowHeightAsk = chartHeight / asksCount

                for (var k = 0; k < asksCount; k++) {
                    var ask = asksModel.get(k)

                    var volPercentAsk = maxVolume > 0 ? (ask.volume / maxVolume) : 0
                    var barWidthAsk = volPercentAsk * (halfWidth - 40)

                    // Координаты для асков
                    var xAsk = leftMargin + halfWidth
                    var yAsk = topMargin + (k * rowHeightAsk)

                    // Градиент фона уровня
                    var gradAsk = ctx.createLinearGradient(xAsk, yAsk, xAsk + barWidthAsk, yAsk)
                    gradAsk.addColorStop(0, "rgba(244, 67, 54, 0.3)")
                    gradAsk.addColorStop(1, "rgba(244, 67, 54, 0.05)")

                    ctx.fillStyle = gradAsk
                    ctx.fillRect(xAsk, yAsk, barWidthAsk, rowHeightAsk - 1)

                    // Текст цены
                    ctx.fillStyle = "#ef5350"
                    ctx.textAlign = "left"
                    ctx.fillText(ask.price.toFixed(2), leftMargin + halfWidth + 5, yAsk + rowHeightAsk / 2)

                    // Текст объема
                    ctx.fillStyle = "#ffffff"
                    ctx.textAlign = "right"
                    ctx.fillText(ask.volume.toFixed(2), leftMargin + chartWidth - 5, yAsk + rowHeightAsk / 2)
                }
            }
        } // onPaint

        // Обновление при изменении моделей
        onWidthChanged: requestPaint()

        onHeightChanged: requestPaint()

    } // orderbookCanvas
} // root

