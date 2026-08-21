import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Application.Core 1.0

Item {
    id: root

    readonly property OrderbookSideModel asks: AppCore.marketState.asks ? AppCore.marketState.asks : null
    readonly property OrderbookSideModel bids: AppCore.marketState.bids ? AppCore.marketState.bids : null
    readonly property real maxVolume: {
        if (!asks || !bids)
            return 0
        return Math.max(asks.maxVolume, bids.maxVolume)
    }

    visible: asks && bids

    Connections {
        target: asks
        function onCountChanged() {
            orderbookCanvas.requestPaint()
            tooltipCanvas.requestPaint()
        }
        function onDataChanged() {
            orderbookCanvas.requestPaint()
            tooltipCanvas.requestPaint()
        }
    }
    Connections {
        target: bids
        function onCountChanged() {
            orderbookCanvas.requestPaint()
            tooltipCanvas.requestPaint()
        }
        function onDataChanged() {
            orderbookCanvas.requestPaint()
            tooltipCanvas.requestPaint()
        }
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
    }

    // Основной Canvas для стакана
    Canvas {
        id: orderbookCanvas
        anchors.top: rowHeader.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        // Храним индексы выделенных уровней и координаты мыши (для подсказки)
        property int hoveredBidIndex: -1
        property int hoveredAskIndex: -1
        property real mouseX: 0
        property real mouseY: 0

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onPositionChanged: function(mouse) {
                if (!bids || !asks)
                    return

                orderbookCanvas.mouseX = mouse.x
                orderbookCanvas.mouseY = mouse.y

                var mx = mouse.x
                var my = mouse.y

                // Отступы и размеры (вычисляем локально)
                var leftMargin = 10
                var rightMargin = 10
                var topMargin = 10
                var bottomMargin = 10
                var chartWidth = orderbookCanvas.width - leftMargin - rightMargin
                var chartHeight = orderbookCanvas.height - topMargin - bottomMargin
                var halfWidth = chartWidth / 2

                // Выход за пределы графика
                if (mx < leftMargin || mx > leftMargin + chartWidth ||
                    my < topMargin || my > topMargin + chartHeight) {
                    orderbookCanvas.hoveredBidIndex = -1
                    orderbookCanvas.hoveredAskIndex = -1
                    orderbookCanvas.requestPaint()
                    tooltipCanvas.requestPaint()
                    return
                }

                var bidsCount = bids.count
                var asksCount = asks.count

                if (mx < leftMargin + halfWidth) {
                    // Bids
                    if (bidsCount === 0) {
                        orderbookCanvas.hoveredBidIndex = -1
                    } else {
                        var rowHeightBid = chartHeight / bidsCount
                        var idx = Math.floor((my - topMargin) / rowHeightBid)
                        if (idx >= 0 && idx < bidsCount) {
                            orderbookCanvas.hoveredBidIndex = idx
                            orderbookCanvas.hoveredAskIndex = -1
                        } else {
                            orderbookCanvas.hoveredBidIndex = -1
                        }
                    }
                } else {
                    // Asks
                    if (asksCount === 0) {
                        orderbookCanvas.hoveredAskIndex = -1
                    } else {
                        var rowHeightAsk = chartHeight / asksCount
                        var idx = Math.floor((my - topMargin) / rowHeightAsk)
                        if (idx >= 0 && idx < asksCount) {
                            orderbookCanvas.hoveredBidIndex = -1
                            orderbookCanvas.hoveredAskIndex = idx
                        } else {
                            orderbookCanvas.hoveredAskIndex = -1
                        }
                    }
                }

                orderbookCanvas.requestPaint()
                tooltipCanvas.requestPaint()
            }
            onExited: {
                orderbookCanvas.hoveredBidIndex = -1
                orderbookCanvas.hoveredAskIndex = -1
                orderbookCanvas.requestPaint()
                tooltipCanvas.requestPaint()
            }
        }

        onPaint: {
            if (!bids || !asks)
                return

            var ctx = getContext("2d")
            ctx.clearRect(0, 0, width, height)

            var leftMargin = 10
            var rightMargin = 10
            var topMargin = 10
            var bottomMargin = 10
            var chartWidth = width - leftMargin - rightMargin
            var chartHeight = height - topMargin - bottomMargin
            var halfWidth = chartWidth / 2

            // Разделительная линия
            ctx.strokeStyle = "#4a4a6a"
            ctx.lineWidth = 1
            ctx.beginPath()
            ctx.moveTo(leftMargin + halfWidth, topMargin)
            ctx.lineTo(leftMargin + halfWidth, topMargin + chartHeight)
            ctx.stroke()

            ctx.font = "11px monospace"
            ctx.textBaseline = "middle"

            // ---- Bids ----
            var bidsCount = bids.count
            if (bidsCount > 0) {
                var rowHeightBid = chartHeight / bidsCount

                // Зоны для Bids: Turnover 20%, Volume 30%, Price 50% (остальное пустое, но Price прижат к центру)
                var turnoverZoneWidth = halfWidth * 0.30
                var volumeZoneWidth = halfWidth * 0.30
                var priceZoneWidth = halfWidth * 0.40   // большая зона, но текст цены рисуется у центра

                var turnoverStartX = leftMargin
                var volumeStartX = leftMargin + turnoverZoneWidth
                var priceStartX = leftMargin + turnoverZoneWidth + volumeZoneWidth

                for (var j = 0; j < bidsCount; j++) {
                    var bid = bids.get(j)
                    var volPercent = maxVolume > 0 ? (bid.volume / maxVolume) : 0
                    var barWidth = volPercent * halfWidth
                    var xBar = leftMargin + halfWidth - barWidth
                    var y = topMargin + j * rowHeightBid

                    // Градиент фона
                    var grad = ctx.createLinearGradient(xBar, y, leftMargin + halfWidth, y)
                    if (j <= orderbookCanvas.hoveredBidIndex) {
                        grad.addColorStop(0, "rgba(76, 175, 80, 0.1)")
                        grad.addColorStop(1, "rgba(76, 175, 80, 0.8)")
                    } else {
                        grad.addColorStop(0, "rgba(76, 175, 80, 0.05)")
                        grad.addColorStop(1, "rgba(76, 175, 80, 0.3)")
                    }
                    ctx.fillStyle = grad
                    ctx.fillRect(xBar, y, barWidth, rowHeightBid - 1)

                    // ---- Тексты ----
                    // 1. Turnover (в левой зоне, выравнивание по левому краю)
                    ctx.fillStyle = "#aaaaaa"
                    ctx.textAlign = "left"
                    var txtTurnover = bid.turnover >= 1000000 ? (bid.turnover / 1000000).toFixed(2) + "M" :
                                        bid.turnover >= 1000 ? (bid.turnover / 1000).toFixed(2) + "K" :
                                        bid.turnover.toFixed(2)
                    ctx.fillText(txtTurnover, turnoverStartX + 5, y + rowHeightBid / 2)

                    // 2. Volume (в средней зоне, выравнивание по левому краю)
                    ctx.fillStyle = "#ffffff"
                    ctx.textAlign = "left"
                    var txtVolume = bid.volume >= 1000000 ? (bid.volume / 1000000).toFixed(2) + "M" :
                                        bid.volume >= 1000 ? (bid.volume / 1000).toFixed(2) + "K" :
                                        bid.volume.toFixed(2)
                    ctx.fillText(txtVolume, volumeStartX + 5, y + rowHeightBid / 2)

                    // 3. Price (в правой зоне, прижато к центру)
                    ctx.fillStyle = "#8bc34a"
                    ctx.textAlign = "right"
                    ctx.fillText(bid.price.toFixed(2), leftMargin + halfWidth - 5, y + rowHeightBid / 2)
                }
            }

            // ---- Asks ----
            var asksCount = asks.count
            if (asksCount > 0) {
                var rowHeightAsk = chartHeight / asksCount

                // Зоны для Asks: Price 20%, Volume 30%, Turnover 50%
                var priceZoneWidthAsk = halfWidth * 0.30
                var volumeZoneWidthAsk = halfWidth * 0.30
                var turnoverZoneWidthAsk = halfWidth * 0.40

                var priceStartXAsk = leftMargin + halfWidth
                var volumeStartXAsk = leftMargin + halfWidth + priceZoneWidthAsk
                var turnoverStartXAsk = leftMargin + halfWidth + priceZoneWidthAsk + volumeZoneWidthAsk

                for (var k = 0; k < asksCount; k++) {
                    var ask = asks.get(k)
                    var volPercent = maxVolume > 0 ? (ask.volume / maxVolume) : 0
                    var barWidth = volPercent * halfWidth
                    var xBar = leftMargin + halfWidth
                    var y = topMargin + k * rowHeightAsk

                    var gradAsk = ctx.createLinearGradient(xBar, y, xBar + barWidth, y)
                    if (k <= orderbookCanvas.hoveredAskIndex) {
                        gradAsk.addColorStop(0, "rgba(244, 67, 54, 0.8)")
                        gradAsk.addColorStop(1, "rgba(244, 67, 54, 0.1)")
                    } else {
                        gradAsk.addColorStop(0, "rgba(244, 67, 54, 0.3)")
                        gradAsk.addColorStop(1, "rgba(244, 67, 54, 0.05)")
                    }
                    ctx.fillStyle = gradAsk
                    ctx.fillRect(xBar, y, barWidth, rowHeightAsk - 1)

                    // ---- Тексты ----
                    // 1. Price (у центра, выравнивание по левому краю)
                    ctx.fillStyle = "#ef5350"
                    ctx.textAlign = "left"
                    ctx.fillText(ask.price.toFixed(2), priceStartXAsk + 5, y + rowHeightAsk / 2)

                    // 2. Volume (в средней зоне)
                    ctx.fillStyle = "#ffffff"
                    ctx.textAlign = "left"
                    var txtVolume = ask.volume >= 1000000 ? (ask.volume / 1000000).toFixed(2) + "M" :
                                        ask.volume >= 1000 ? (ask.volume / 1000).toFixed(2) + "K" :
                                        ask.volume.toFixed(2)
                    ctx.fillText(txtVolume, volumeStartXAsk + 5, y + rowHeightAsk / 2)

                    // 3. Turnover (в правой зоне, выравнивание по левому краю)
                    ctx.fillStyle = "#aaaaaa"
                    ctx.textAlign = "left"
                    var txtTurnover = ask.turnover >= 1000000 ? (ask.turnover / 1000000).toFixed(2) + "M" :
                                        ask.turnover >= 1000 ? (ask.turnover / 1000).toFixed(2) + "K" :
                                        ask.turnover.toFixed(2)
                    ctx.fillText(txtTurnover, turnoverStartXAsk + 5, y + rowHeightAsk / 2)
                }
            }

            // ---- Выделение диапазона ----
            if (orderbookCanvas.hoveredBidIndex >= 0 && bidsCount > 0) {
                var totalHeightBid = (orderbookCanvas.hoveredBidIndex + 1) * rowHeightBid
                ctx.strokeStyle = "#8bc34a"
                ctx.lineWidth = 1
                ctx.strokeRect(leftMargin, topMargin, halfWidth, totalHeightBid)
                ctx.fillStyle = "rgba(139, 195, 74, 0.08)"
                ctx.fillRect(leftMargin, topMargin, halfWidth, totalHeightBid)
            } else if (orderbookCanvas.hoveredAskIndex >= 0 && asksCount > 0) {
                var totalHeightAsk = (orderbookCanvas.hoveredAskIndex + 1) * rowHeightAsk
                ctx.strokeStyle = "#ef5350"
                ctx.lineWidth = 1
                ctx.strokeRect(leftMargin + halfWidth, topMargin, halfWidth, totalHeightAsk)
                ctx.fillStyle = "rgba(244, 67, 54, 0.08)"
                ctx.fillRect(leftMargin + halfWidth, topMargin, halfWidth, totalHeightAsk)
            }
        } // orderbookCanvas onPaint

        onWidthChanged: requestPaint()
        onHeightChanged: requestPaint()
    }

    // Дополнительный Canvas для всплывающей подсказки (tooltip)
    Canvas {
        id: tooltipCanvas
        anchors.fill: parent
        z: 1
        enabled: false

        onPaint: {
            if (!bids || !asks)
                return

            var ctx = getContext("2d")
            ctx.clearRect(0, 0, width, height)

            // Проверяем, есть ли выделенный уровень
            var levelData = null
            var isBid = false
            if (orderbookCanvas.hoveredBidIndex >= 0 && bids.count > 0) {
                levelData = bids.get(orderbookCanvas.hoveredBidIndex)
                isBid = true
            } else if (orderbookCanvas.hoveredAskIndex >= 0 && asks.count > 0) {
                levelData = asks.get(orderbookCanvas.hoveredAskIndex)
                isBid = false
            }

            if (!levelData)
                return

            var txtTotalVolume = levelData.totalVolume >= 1000000 ? (levelData.totalVolume / 1000000).toFixed(2) + "M" :
                                levelData.totalVolume >= 1000 ? (levelData.totalVolume / 1000).toFixed(2) + "K" :
                                levelData.totalVolume.toFixed(2)

            var txtTotalTurnover = levelData.totalTurnover >= 1000000 ? (levelData.totalTurnover / 1000000).toFixed(2) + "M" :
                                levelData.totalTurnover >= 1000 ? (levelData.totalTurnover / 1000).toFixed(2) + "K" :
                                levelData.totalTurnover.toFixed(2)

            var lines = [
                "Avg. Price:     " + levelData.avgPrice.toFixed(2),
                "Total Volume:   " + txtTotalVolume,
                "Total Turnover: " + txtTotalTurnover
            ]

            var padding = 8
            var lineHeight = 16
            ctx.font = "11px monospace"
            ctx.textBaseline = "top"

            // Вычисляем размеры панели
            var maxWidth = 0
            for (var i = 0; i < lines.length; i++) {
                var w = ctx.measureText(lines[i]).width
                if (w > maxWidth) maxWidth = w
            }
            var panelWidth = maxWidth + padding * 2
            var panelHeight = lines.length * lineHeight + padding * 2

            // Позиция около курсора
            var panelX = orderbookCanvas.mouseX + 12
            var panelY = orderbookCanvas.mouseY - 10

            // Корректировка, чтобы не выходить за границы
            if (panelX + panelWidth > width) panelX = orderbookCanvas.mouseX - panelWidth - 12
            if (panelY + panelHeight > height) panelY = height - panelHeight - 5
            if (panelY < 0) panelY = 5
            if (panelX < 0) panelX = 5

            // Тень
            ctx.shadowColor = "rgba(0, 0, 0, 0.5)"
            ctx.shadowBlur = 10

            // Фон
            ctx.fillStyle = "rgba(30, 30, 40, 0.92)"
            ctx.beginPath()
            ctx.rect(panelX, panelY, panelWidth, panelHeight)
            ctx.fill()

            ctx.shadowBlur = 0

            // Рамка
            ctx.strokeStyle = isBid ? "#8bc34a" : "#ef5350"
            ctx.lineWidth = 1
            ctx.strokeRect(panelX, panelY, panelWidth, panelHeight)

            // Текст
            ctx.fillStyle = "white"
            ctx.textAlign = "left"
            for (var i = 0; i < lines.length; i++)
                ctx.fillText(lines[i], panelX + padding, panelY + padding + i * lineHeight)

        }

        onWidthChanged: requestPaint()
        onHeightChanged: requestPaint()
    }
}