import QtQuick 2.15
import Application.Core 1.0

Rectangle {
    id: root

    color: "#1e1e1e"
    border.width: 1
    border.color: "#3a3a3a"

    // Свойства
    readonly property var klineSeries: AppCore.marketState.klineSeries

    property bool enableAutoScroll: true
    property bool enableCursorTime: true
    property bool enableCursorPrice: true
    property bool enableCursorVolume: true
    property bool enableCursorInfo: true
    property bool enableDisplayOpenPrice: true
    property bool enableShowVolumes: true

    property int timeAutoScroll: 3000
    property real volumeChartHeightRatio: 0.25

    signal leftBoundaryReached()

    function zoomIn() {
        var newWidth = Math.min(internal.candleWidth + internal.zoomStep, internal.maxCandleWidth);

        if (newWidth !== internal.candleWidth) {
            var oldX = priceChartScrollView.contentX
            internal.updatingScroll = true

            internal.candleWidth = newWidth
            priceChartScrollView.contentWidth = Math.max(root.width, internal.totalChartWidth)

            var maxScrollX = Math.max(0, priceChartScrollView.contentWidth - priceChartScrollView.width);
            if (maxScrollX <= 0) {
                priceChartScrollView.contentX = 0;
            } else {
                var newX = Math.min(oldX, maxScrollX);
                priceChartScrollView.contentX = Math.max(0, newX);
            }

            internal.saveScrollPosition();
            internal.updatingScroll = false;
            internal.syncScrollViews();
            internal.updateVisibleRange();
        }
    }

    function zoomOut() {
        var newWidth = Math.max(internal.candleWidth - internal.zoomStep, internal.minCandleWidth);

        if (newWidth !== internal.candleWidth) {
            var oldX = priceChartScrollView.contentX
            internal.updatingScroll = true

            internal.candleWidth = newWidth
            priceChartScrollView.contentWidth = Math.max(root.width, internal.totalChartWidth)

            var maxScrollX = Math.max(0, priceChartScrollView.contentWidth - priceChartScrollView.width);
            if (maxScrollX <= 0) {
                priceChartScrollView.contentX = 0;
            } else {
                var newX = Math.min(oldX, maxScrollX);
                priceChartScrollView.contentX = Math.max(0, newX);
            }

            internal.saveScrollPosition();
            internal.updatingScroll = false;
            internal.syncScrollViews();
            internal.updateVisibleRange();
        }
    }

    function resetZoom() {
        var oldX = priceChartScrollView.contentX;
        internal.updatingScroll = true;

        internal.candleWidth = 20;
        priceChartScrollView.contentWidth = Math.max(root.width, internal.totalChartWidth);

        var maxScrollX = Math.max(0, priceChartScrollView.contentWidth - priceChartScrollView.width);
        if (maxScrollX <= 0) {
            priceChartScrollView.contentX = 0;
        } else {
            var newX = Math.min(oldX, maxScrollX);
            priceChartScrollView.contentX = Math.max(0, newX);
        }

        internal.saveScrollPosition();
        internal.updatingScroll = false;
        internal.syncScrollViews();
        internal.updateVisibleRange();

    }

    QtObject {
        id: internal

        property int candleWidth: 20 // Ширина свечи
        property int candleSpacing: 6 // Оступы между свечой
        property int priceAxisWidth: 60 // Ширина оси цен
        property int timeAxisHeight: 40 // Высота оси времени
        property int totalChartWidth: klineSeries ? klineSeries.length * (candleWidth + candleSpacing) : 0 // Общая ширина графика
        property bool autoScrollEnabled: true // Флаг автоматической прокрутки к последней свече
        property bool isUserInteracting: false // Флаг, показывающий, что идет ручная прокрутка
        property bool isScrolling: false // Флаг скроллинга

        // Настройки зума
        property int minCandleWidth: 2
        property int maxCandleWidth: 80
        property int zoomStep: 2

        // Сохраняем позицию
        property real scrollPosition: 0.0
        property bool updatingScroll: false
        property int firstVisibleIdx: 0
        property int lastVisibleIdx: -1

        // Свойства для кросс-курсора
        property real mouseGlobalX: -1
        property real mouseGlobalY: -1
        property real mouseLocalX: -1
        property real mouseLocalY: -1
        property bool mouseInsidePriceChart: false
        property bool mouseInsideVolumeChart: false
        property bool mouseInsideTimeAxis: false
        property bool mouseInside: mouseInsidePriceChart || mouseInsideVolumeChart || mouseInsideTimeAxis
        property int hoveredCandleIndex: -1

        // Параметры новеденной свечи
        property var hoveredCandle: {
            if (hoveredCandleIndex < 0 || !klineSeries)
                return null

            var obj = klineSeries.get(hoveredCandleIndex)

            if (isValid(obj))
                return obj


            return null
        } //klineSeries ? klineSeries.get(hoveredCandleIndex) : null
        property color colorHoveredCandle: hoveredCandle ?
            ((hoveredCandle.close > hoveredCandle.open) ? "#66BB6A" : "#EF5350") :
            "#8a8a8a"
        property var openHoveredCandle: hoveredCandle ? hoveredCandle.open.toFixed(2) : "-"
        property var closeHoveredCandle: hoveredCandle ? hoveredCandle.close.toFixed(2) : "-"
        property var highHoveredCandle: hoveredCandle ? hoveredCandle.high.toFixed(2) : "-"
        property var lowHoveredCandle: hoveredCandle ? hoveredCandle.low.toFixed(2) : "-"
        property var difHoveredCandle: hoveredCandle ? (hoveredCandle.close - hoveredCandle.open).toFixed(2) : "-"
        property var difPercentHoveredCandle: hoveredCandle ? ((difHoveredCandle / hoveredCandle.open) * 100).toFixed(2) : "-"
        property var volumeHoveredCandle: hoveredCandle ?
            hoveredCandle.volume >= 1000000 ? (hoveredCandle.volume / 1000000).toFixed(2) + "M" :
            hoveredCandle.volume >= 1000 ? (hoveredCandle.volume / 1000).toFixed(2) + "K" :
            hoveredCandle.volume.toFixed(2) : "-"
        property var turnoverHoveredCandle: hoveredCandle ?
            hoveredCandle.turnover >= 1000000 ? (hoveredCandle.turnover / 1000000).toFixed(2) + "M" :
            hoveredCandle.turnover >= 1000 ? (hoveredCandle.turnover / 1000).toFixed(2) + "K" :
            hoveredCandle.turnover.toFixed(2) : "-"

        // Данные на осях
        property real priceAtCursor: 0
        property real volumeAtCursor: 0
        property string timeAtCursor: ""

        // Предельные значения
        property real maxPrice: 100
        property real minPrice: 0
        property real maxVolume: 1

        function isValid(candle)
        {
            //console.log(JSON.stringify(candle))
            return candle &&
                    typeof candle.open === 'number' &&
                    typeof candle.close === 'number' &&
                    typeof candle.high === 'number' &&
                    typeof candle.low === 'number' &&
                    typeof candle.volume === 'number' &&
                    typeof candle.turnover === 'number' &&
                    typeof candle.time === 'number' &&
                    typeof candle.isConfirm === 'boolean'
        }

        function isCanvasReady(canvas)
        {
            return canvas &&
                    canvas.status === Canvas.Ready &&
                    canvas.width > 0 &&
                    canvas.height > 0 &&
                    canvas.visible
        }

        function safeRequestPaint(canvas)
        {
            // if (!canvas)
            // {
            //     Qt.callLater(safeRequestPaint, canvas)
            //     return
            // }

            // if (isCanvasReady(canvas))
            // {
            //     canvas.requestPaint()
            // }
            // else
            // {
            //     var onStatusChanged = function() {
            //         if (isCanvasReady(canvas))
            //         {
            //             canvas.requestPaint()
            //             canvas.statusChanged.disconnect(onStatusChanged)
            //         }
            //     }
            //     canvas.statusChanged.connect(onStatusChanged)
            // }

            var canvasName = canvas.objectName || "Unnamed Canvas"
            var funcName = arguments.callee.name

            if (isCanvasReady(canvas))
            {
                //console.log("[" + funcName + "] Successful rendering: " + canvasName)
                canvas.requestPaint()
            }
            else
            {
                //console.log("[" + funcName + "] Deferred rendering: " + canvasName)
                Qt.callLater(safeRequestPaint, canvas)
            }
        }

        // Функция для рассчета шага сетки
        function calculateGridStep(range, targetLines) {
            if (range <= 0)
                return 1.0;

            var rawStep = range / targetLines;
            var log10 = Math.log(rawStep) / Math.LN10;
            var orderOfMagnitude = Math.pow(10, Math.floor(log10));
            var normalizedStep = rawStep / orderOfMagnitude;

            var cleanStep;
            if (normalizedStep < 1.5)
                cleanStep = 1.0;
            else if (normalizedStep < 3.0)
                cleanStep = 2.0;
            else if (normalizedStep < 7.0)
                cleanStep = 5.0;
            else
                cleanStep = 10.0;

            return cleanStep * orderOfMagnitude;
        }

        // Функция для получения видимых индексов на графике
        function getVisibleCandleIndices() {
            if (!root.klineSeries || root.klineSeries.length === 0)
                return { start: 0, end: -1 }

            var step = internal.candleWidth + internal.candleSpacing
            var effectiveX = Math.max(0, priceChartScrollView.contentX)
            var buffer = 2
            var firstIdx = Math.max(0, Math.floor(effectiveX / step) - buffer)
            var lastIdx = Math.min(root.klineSeries.length - 1,
                                   Math.ceil((effectiveX + priceChartScrollView.width) / step) + buffer)
            return { start: firstIdx, end: lastIdx }
        }

        // Обновляет макс и мин значения в облости видимых индексов и вызывает перерисовку всех компонентов
        function updateVisibleRange() {
            if (scrollAnimation.running)
                return;

            if (!root.klineSeries || root.klineSeries.length === 0)
                return;


            var visibleIdx = getVisibleCandleIndices();
            internal.firstVisibleIdx = visibleIdx.start;
            internal.lastVisibleIdx = visibleIdx.end;

            var currentMax = -Infinity;
            var currentMin = Infinity;
            var currentMaxVolume = 0;

            for (var i = internal.firstVisibleIdx; i <= internal.lastVisibleIdx; i++) {
                var candle = root.klineSeries.get(i);
                if (candle) {
                    if (candle.high > currentMax)
                        currentMax = candle.high;
                    if (candle.low < currentMin)
                        currentMin = candle.low;
                    if (candle.volume > currentMaxVolume)
                        currentMaxVolume = candle.volume;
                }
            }

            if (currentMax !== -Infinity && currentMin !== Infinity && currentMax >= currentMin) {
                var priceRange = currentMax - currentMin;
                var padding = priceRange * 0.15;
                if (padding === 0)
                    padding = 1.0;
                internal.maxPrice = currentMax + padding;
                internal.minPrice = currentMin - padding;
            }

            if (currentMaxVolume > internal.maxVolume)
                internal.maxVolume = currentMaxVolume * 1.5;

            requestPaintAll();
        }

        // Перематывает к последней свече с обновлением видимого диапозона
        function scrollToLastCandle(animated) {
            if (!root.klineSeries || root.klineSeries.length === 0)
                return;

            var maxScrollX = Math.max(0, priceChartScrollView.contentWidth - priceChartScrollView.width);

            if (animated) {
                internal.updatingScroll = true;
                scrollAnimation.to = maxScrollX;
                scrollAnimation.start();
            } else {
                internal.updatingScroll = true;
                priceChartScrollView.contentX = maxScrollX;
                Qt.callLater(function() {
                    internal.syncScrollViews()
                    internal.updateVisibleRange()
                    internal.updatingScroll = false
                });
            }
        }

        // Получает индекс свечи по координате x
        function getCandleAtX(x) {
            if (!root.klineSeries || root.klineSeries.length === 0)
                return null;

            var step = internal.candleWidth + internal.candleSpacing
            var index = Math.floor((x - internal.candleSpacing / 2) / step)

            if (index < 0 || index >= root.klineSeries.length)
                return null;

            var candleStartX = index * step + internal.candleSpacing / 2
            var candleEndX = candleStartX + internal.candleWidth

            if (x < candleStartX || x > candleEndX)
                return null;

            return { index: index };
        }

        function convertPriceToY(price, availableHeight) {
            var range = internal.maxPrice - internal.minPrice
            if (range === 0)
                return availableHeight / 2;
            return availableHeight - ((price - internal.minPrice) / range) * availableHeight;
        }

        function convertYToPrice(y, availableHeight) {
            var range = internal.maxPrice - internal.minPrice;
            if (range === 0)
                return internal.maxPrice;
            return internal.maxPrice - (y / availableHeight) * range;
        }

        function convertYToVolume(y, availableHeight) {
            if (internal.maxVolume === 0)
                return internal.maxVolume;
            return internal.maxVolume - (y / availableHeight) * internal.maxVolume;
        }

        function convertXToTime(x) {
            if (!root.klineSeries || root.klineSeries.length === 0)
                return -1
            var result = getCandleAtX(x)
            if (result && result.index >= 0 && result.index < root.klineSeries.length) {
                var candle = root.klineSeries.get(result.index);
                if (!candle)
                    return -1
                return (candle.time !== undefined) ? candle.time : -1
            }
            return -1
        }

        function updateAxes() {
            safeRequestPaint(priceCanvas)
            safeRequestPaint(volumeCanvas)
            safeRequestPaint(timeCanvas)
        }

        function updateCharts() {
            safeRequestPaint(priceChartCanvas)
            safeRequestPaint(volumeChartCanvas)
        }

        function updateCrosshair() {
            if (!internal.mouseInside || internal.mouseGlobalX < 0 || internal.mouseGlobalY < 0) {
                safeRequestPaint(crosshairCanvas)
                return
            }

            var result = internal.getCandleAtX(internal.mouseGlobalX);
            if (result && result.index >= 0 && result.index < root.klineSeries.length) {
                internal.hoveredCandleIndex = result.index;
                var candle = root.klineSeries.get(result.index);
            } else
                internal.hoveredCandleIndex = -1;

            var timestamp = internal.convertXToTime(internal.mouseGlobalX)
            internal.timeAtCursor = (timestamp > 0) ? Qt.formatDateTime(new Date(timestamp), "dd.MM.yyyy HH:mm") : ""
            internal.priceAtCursor = internal.mouseInsidePriceChart ? internal.convertYToPrice(internal.mouseGlobalY, priceChartScrollView.height) : 0
            internal.volumeAtCursor = internal.mouseInsideVolumeChart ? internal.convertYToVolume(internal.mouseGlobalY - priceChartScrollView.height, volumeChartScrollView.height) : 0

            safeRequestPaint(crosshairCanvas)
        }

        function saveScrollPosition() {
            var maxScrollX = Math.max(1, priceChartScrollView.contentWidth - priceChartScrollView.width);
            if (maxScrollX > 0) {
                internal.scrollPosition = priceChartScrollView.contentX / maxScrollX;
                internal.scrollPosition = Math.max(0, Math.min(1, internal.scrollPosition));
            } else {
                internal.scrollPosition = 0;
            }
        }

        function restoreScrollPosition(animated) {
            var maxScrollX = Math.max(0, priceChartScrollView.contentWidth - priceChartScrollView.width)
            var targetX = internal.scrollPosition * maxScrollX
            targetX = Math.max(0, Math.min(targetX, maxScrollX))

            if (animated) {
                internal.updatingScroll = true
                scrollAnimation.to = targetX
                scrollAnimation.start()
            } else {
                internal.updatingScroll = true
                priceChartScrollView.contentX = targetX
                internal.updatingScroll = false
            }
        }

        function updateScrollPosition() {
            var maxScrollX = Math.max(1, priceChartScrollView.contentWidth - priceChartScrollView.width);
            if (!internal.updatingScroll && maxScrollX > 0) {
                internal.scrollPosition = priceChartScrollView.contentX / maxScrollX;
                internal.scrollPosition = Math.max(0, Math.min(1, internal.scrollPosition));
            }
        }

        function requestPaintAll() {
            console.log("[" + arguments.callee.name + "]")

            if (root.width === 0 || root.height === 0)
                return

            var canvases = [
                priceChartCanvas,
                volumeChartCanvas,
                priceCanvas,
                volumeCanvas,
                timeCanvas,
                crosshairCanvas
            ]

            canvases.forEach(function(canvas) { safeRequestPaint(canvas) })
        }

        function update() {
            if (root.width === 0 || root.height === 0)
                return;

            if (!root.klineSeries || root.klineSeries.length === 0) {
                requestPaintAll();
                return;
            }

            // Устанавливаем ширину контента
            priceChartScrollView.contentWidth = Math.max(root.width, internal.totalChartWidth);
            // Синхронизируем ширину для графика объемов
            syncScrollViews()

            // Если включена автопрокрутка и контент шире видимой области – скроллим к последней свече
            if (internal.autoScrollEnabled && internal.totalChartWidth > priceChartScrollView.width) {
                internal.scrollToLastCandle(false)
                return
            }

            // Отложенная перерисовка для гарантии
            Qt.callLater(function() {
                // Принудительно обновляем видимый диапазон
                internal.updateVisibleRange()
            });
        }

        // Синхронизация скролла между графиками
        function syncScrollViews() {
            var safeX = Math.max(0, priceChartScrollView.contentX);
            if (volumeChartScrollView.contentX !== safeX)
                volumeChartScrollView.contentX = safeX;
            volumeChartScrollView.contentWidth = priceChartScrollView.contentWidth;
        }
    }

    NumberAnimation {
        id: scrollAnimation
        target: priceChartScrollView
        property: "contentX"
        duration: 200
        easing.type: Easing.OutCubic
        onStopped: {
            internal.syncScrollViews()
            internal.updateVisibleRange()
            internal.updatingScroll = false
        }
    } // scrollAnimation

    Timer {
        id: autoScrollRestartTimer
        interval: root.timeAutoScroll

        onTriggered: {
            if (root.enableAutoScroll) {
                internal.autoScrollEnabled = true;
                if (internal.totalChartWidth > priceChartScrollView.width)
                {
                    console.log("Timer trigged")
                    internal.scrollToLastCandle(true)
                }
            }
        }
    } // autoScrollRestartTimer

    MouseArea {
        id: zoomArea
        anchors.fill: parent
        acceptedButtons: Qt.NoButton
        hoverEnabled: true
        preventStealing: true

        onWheel: function(wheel) {
            var hasData = root.klineSeries && root.klineSeries.length > 0;
            if (!hasData) {
                wheel.accepted = false;
                return;
            }

            if (wheel.modifiers & Qt.ControlModifier) {
                if (wheel.angleDelta.y > 0)
                    root.zoomIn()
                else
                    root.zoomOut()
                wheel.accepted = true
            } else {
                var canScroll = internal.totalChartWidth > priceChartScrollView.width;
                if (canScroll) {
                    var delta = wheel.angleDelta.y / 120 * 50;
                    var newX = priceChartScrollView.contentX - delta;
                    var maxScrollX = Math.max(0, priceChartScrollView.contentWidth - priceChartScrollView.width);
                    priceChartScrollView.contentX = Math.max(0, Math.min(newX, maxScrollX));
                    internal.syncScrollViews();
                    wheel.accepted = true;
                } else
                    wheel.accepted = false;
            }
        }

        onPositionChanged: function(mouse) {
            internal.mouseInsidePriceChart = mouse.x >= priceArea.x && mouse.y >= priceArea.y &&
                                            mouse.x <= priceArea.x + priceArea.width && mouse.y <= priceArea.y + priceArea.height

            internal.mouseInsideVolumeChart = mouse.x >= volumeArea.x && mouse.y >= volumeArea.y &&
                                            mouse.x <= volumeArea.x + volumeArea.width && mouse.y <= volumeArea.y + volumeArea.height

            internal.mouseInsideTimeAxis = mouse.x >= timeAxisArea.x && mouse.y >= timeAxisArea.y &&
                                            mouse.x <= timeAxisArea.x + timeAxisArea.width && mouse.y <= timeAxisArea.y + timeAxisArea.height

            internal.mouseGlobalX = mouse.x + priceChartScrollView.contentX
            internal.mouseGlobalY = mouse.y + priceChartScrollView.contentY
            internal.mouseLocalX = mouse.x
            internal.mouseLocalY = mouse.y

            /*console.log(" - Local:", internal.mouseLocalX, internal.mouseLocalY,
                        "Global:", internal.mouseGlobalX, internal.mouseGlobalY)
            console.log("Price chart:", priceArea.x, priceArea.y, priceArea.width, priceArea.height)
            console.log("Volume chart:", volumeArea.x, volumeArea.y, volumeArea.width, volumeArea.height)
            console.log("Inside price chart:", internal.mouseInsidePriceChart,
                        "Inside volume chart:", internal.mouseInsideVolumeChart,
                        "Inside:", internal.mouseInside)*/

            internal.updateCrosshair()
            internal.updateAxes()
        }

        onExited: {
            internal.mouseInsidePriceChart = false
            internal.mouseInsideVolumeChart = false
            internal.mouseInsideTimeAxis = false

            internal.mouseGlobalX = -1
            internal.mouseGlobalY = -1
            internal.mouseLocalX = -1
            internal.mouseLocalY = -1
            internal.hoveredCandleIndex = -1

            internal.priceAtCursor = 0
            internal.volumeAtCursor = 0
            internal.timeAtCursor = ""

            internal.updateCrosshair()
            internal.updateAxes()
        }
    } // zoomArea

    Canvas {
        id: crosshairCanvas
        objectName: "crosshairCanvas"
        anchors.fill: parent
        z: 10

        Component.onCompleted: console.log("crosshairCanvas component ready:", width, height)

        function drawCursorLines(ctx) {
            ctx.strokeStyle = "rgba(255, 255, 255, 0.3)"
            ctx.lineWidth = 1
            ctx.setLineDash([5, 5])

            // Вертикальная линия
            ctx.beginPath()
            ctx.moveTo(internal.mouseLocalX, 0)
            ctx.lineTo(internal.mouseLocalX, height)
            ctx.stroke()

            // Горизонтальная линия
            ctx.beginPath()
            ctx.moveTo(0, internal.mouseLocalY)
            ctx.lineTo(width, internal.mouseLocalY)
            ctx.stroke()
        }

        function drawCursorIndicator(ctx) {
            ctx.beginPath();
            ctx.arc(internal.mouseLocalX, internal.mouseLocalY, 4, 0, Math.PI * 2);
            ctx.fillStyle = "rgba(255, 255, 255, 0.5)";
            ctx.fill();
            ctx.strokeStyle = "white";
            ctx.lineWidth = 1;
            ctx.stroke();
        }

        function drawCursorPrice(ctx) {
            if (internal.mouseInsidePriceChart) {
                var cursorY = internal.mouseLocalY

                var priceText = internal.priceAtCursor.toFixed(2);
                ctx.font = "bold 10px sans-serif";
                var textWidth = ctx.measureText(priceText).width + 10;
                var textHeight = 16;

                var textX = width - textWidth - 2;
                var textY = cursorY - textHeight / 2;

                if (textY < 0)
                    textY = 0;
                if (textY + textHeight > height)
                    textY = height - textHeight;

                ctx.fillStyle = "rgba(30, 30, 30, 0.85)";
                ctx.strokeStyle = "rgba(255, 255, 255, 0.2)";
                ctx.lineWidth = 1;

                var radius = 3;
                ctx.beginPath();
                ctx.moveTo(textX + radius, textY);
                ctx.lineTo(textX + textWidth - radius, textY);
                ctx.quadraticCurveTo(textX + textWidth, textY, textX + textWidth, textY + radius);
                ctx.lineTo(textX + textWidth, textY + textHeight - radius);
                ctx.quadraticCurveTo(textX + textWidth, textY + textHeight, textX + textWidth - radius, textY + textHeight);
                ctx.lineTo(textX + radius, textY + textHeight);
                ctx.quadraticCurveTo(textX, textY + textHeight, textX, textY + textHeight - radius);
                ctx.lineTo(textX, textY + radius);
                ctx.quadraticCurveTo(textX, textY, textX + radius, textY);
                ctx.closePath();
                ctx.fill();
                ctx.stroke();

                ctx.fillStyle = "#ffffff";
                ctx.font = "bold 10px sans-serif";
                ctx.textBaseline = "middle";
                ctx.fillText(priceText, textX + 5, textY + textHeight / 2);
            }
        }

        function drawCursorVolume(ctx) {
            if (internal.mouseInsideVolumeChart) {
                var cursorY = internal.mouseLocalY

                var volumeText = internal.volumeAtCursor.toFixed(2);
                ctx.font = "bold 10px sans-serif";
                var textWidth = ctx.measureText(volumeText).width + 10;
                var textHeight = 16;

                var textX = width - textWidth - 2;
                var textY = cursorY - textHeight / 2;

                if (textY < 0)
                    textY = 0;
                if (textY + textHeight > height)
                    textY = height - textHeight;

                ctx.fillStyle = "rgba(30, 30, 30, 0.85)";
                ctx.strokeStyle = "rgba(255, 255, 255, 0.2)";
                ctx.lineWidth = 1;

                var radius = 3;
                ctx.beginPath();
                ctx.moveTo(textX + radius, textY);
                ctx.lineTo(textX + textWidth - radius, textY);
                ctx.quadraticCurveTo(textX + textWidth, textY, textX + textWidth, textY + radius);
                ctx.lineTo(textX + textWidth, textY + textHeight - radius);
                ctx.quadraticCurveTo(textX + textWidth, textY + textHeight, textX + textWidth - radius, textY + textHeight);
                ctx.lineTo(textX + radius, textY + textHeight);
                ctx.quadraticCurveTo(textX, textY + textHeight, textX, textY + textHeight - radius);
                ctx.lineTo(textX, textY + radius);
                ctx.quadraticCurveTo(textX, textY, textX + radius, textY);
                ctx.closePath();
                ctx.fill();
                ctx.stroke();

                ctx.fillStyle = "#ffffff";
                ctx.font = "bold 10px sans-serif";
                ctx.textBaseline = "middle";
                ctx.fillText(volumeText, textX + 5, textY + textHeight / 2);
            }
        }

        function drawCursorTime(ctx) {
            var containerWidth = width;
            var step = internal.candleWidth + internal.candleSpacing;
            if (internal.mouseInside && internal.mouseLocalX >= 0 && internal.mouseLocalX <= containerWidth && internal.timeAtCursor !== "") {
                var result = internal.getCandleAtX(internal.mouseLocalX);
                if (result && result.index >= 0 && result.index < root.klineSeries.length) {
                    var candleIndex = result.index;
                    var candleXPos = candleIndex * step + internal.candleSpacing / 2;
                    var candleCenterX = candleXPos + internal.candleWidth / 2;

                    var timeText = internal.timeAtCursor;
                    ctx.font = "bold 10px sans-serif";
                    var textWidth = ctx.measureText(timeText).width + 12;
                    var textHeight = 18;

                    var textX = candleCenterX - textWidth / 2;
                    var textY = height - textHeight - 2;

                    if (textX < 0)
                        textX = 0;
                    if (textX + textWidth > width)
                        textX = width - textWidth;

                    ctx.fillStyle = "rgba(30, 30, 30, 0.85)";
                    ctx.strokeStyle = "rgba(255, 255, 255, 0.2)";
                    ctx.lineWidth = 1;

                    var radius = 3;
                    ctx.beginPath();
                    ctx.moveTo(textX + radius, textY);
                    ctx.lineTo(textX + textWidth - radius, textY);
                    ctx.quadraticCurveTo(textX + textWidth, textY, textX + textWidth, textY + radius);
                    ctx.lineTo(textX + textWidth, textY + textHeight - radius);
                    ctx.quadraticCurveTo(textX + textWidth, textY + textHeight, textX + textWidth - radius, textY + textHeight);
                    ctx.lineTo(textX + radius, textY + textHeight);
                    ctx.quadraticCurveTo(textX, textY + textHeight, textX, textY + textHeight - radius);
                    ctx.lineTo(textX, textY + radius);
                    ctx.quadraticCurveTo(textX, textY, textX + radius, textY);
                    ctx.closePath();
                    ctx.fill();
                    ctx.stroke();

                    ctx.fillStyle = "#ffffff";
                    ctx.font = "bold 10px sans-serif";
                    ctx.textAlign = "center";
                    ctx.textBaseline = "middle";
                    ctx.fillText(timeText, textX + textWidth / 2, textY + textHeight / 2);
                }
            }
        }

        Connections {
            target: internal
            function onMouseGlobalXChanged() {
                internal.safeRequestPaint(crosshairCanvas)
            }
            function onMouseGlobalYChanged() {
                internal.safeRequestPaint(crosshairCanvas)
            }
            function onHoveredCandleIndexChanged() {
                internal.safeRequestPaint(crosshairCanvas)
            }
            function onMouseInsideChanged() {
                internal.safeRequestPaint(crosshairCanvas)
            }
            function onTimeAtCursorChanged() {
                internal.safeRequestPaint(crosshairCanvas)
            }
            function onPriceAtCursorChanged() {
                internal.safeRequestPaint(crosshairCanvas)
            }
        }

        onPaint: {
            if (width === 0 || height === 0)
                return;

            var ctx = getContext("2d");
            ctx.clearRect(0, 0, width, height)

            if (!internal.mouseInside || internal.mouseGlobalX < 0 || internal.mouseGlobalY < 0)
                return;

            ctx.save()
            ctx.scale(1,1)

            drawCursorLines(ctx)
            ctx.setLineDash([]);
            drawCursorIndicator(ctx)

            if (root.enableCursorPrice)
                drawCursorPrice(ctx)

            if (root.enableCursorVolume)
                drawCursorVolume(ctx)

            if (root.enableCursorTime)
                drawCursorTime(ctx)


            ctx.restore();
        }
    } // crosshairCanvas

    // Область ценового графика (верхняя часть)
    Rectangle {
        id: priceArea
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        height: root.enableShowVolumes ? (parent.height - internal.timeAxisHeight) * (1 - root.volumeChartHeightRatio)
                                 : parent.height - internal.timeAxisHeight
        color: "transparent"

        // Обработчики изменения размеров для принудительной перерисовки
        onWidthChanged: {
            if (width === 0 || height === 0)
                return;
            internal.safeRequestPaint(priceCanvas)
            internal.safeRequestPaint(priceChartCanvas)
        }
        onHeightChanged: {
            if (width === 0 || height === 0)
                return;
            internal.safeRequestPaint(priceCanvas)
            internal.safeRequestPaint(priceChartCanvas)
        }

        // Ценовая информация о свече
        Rectangle {
            anchors.left: parent.left
            anchors.top: parent.top
            width: priceInfoLayout.width + 16
            height: priceInfoLayout.height + 16
            color: "transparent"
            enabled: root.enableCursorInfo
            z: 20

            Row {
                id: priceInfoLayout
                anchors.centerIn: parent
                spacing: 10
                padding: 2

                Text {
                    text: "O:"
                    color: "#8a8a8a"
                    font.pixelSize: 11
                    font.family: "monospace"
                }
                Text {
                    text: internal.openHoveredCandle
                    color: internal.colorHoveredCandle
                    font.pixelSize: 11
                    font.family: "monospace"
                }

                Text {
                    text: "H:"
                    color: "#8a8a8a"
                    font.pixelSize: 11
                    font.family: "monospace"
                }
                Text {
                    text: internal.highHoveredCandle
                    color: internal.colorHoveredCandle
                    font.pixelSize: 11
                    font.family: "monospace"
                }

                Text {
                    text: "L:"
                    color: "#8a8a8a"
                    font.pixelSize: 11
                    font.family: "monospace"
                }
                Text {
                    text: internal.lowHoveredCandle
                    color: internal.colorHoveredCandle
                    font.pixelSize: 11
                    font.family: "monospace"
                }

                Text {
                    text: "C:"
                    color: "#8a8a8a"
                    font.pixelSize: 11
                    font.family: "monospace"
                }
                Text {
                    text: internal.closeHoveredCandle
                    color: internal.colorHoveredCandle
                    font.pixelSize: 11
                    font.family: "monospace"
                }

                Text {
                    text: "Δ:"
                    color: "#8a8a8a"
                    font.pixelSize: 11
                    font.family: "monospace"
                }
                Text {
                    text: internal.difHoveredCandle + " (" + internal.difPercentHoveredCandle + "%)"
                    color: internal.colorHoveredCandle
                    font.pixelSize: 11
                    font.family: "monospace"
                }
            } // priceInfoLayout
        }

        // Правая ось цен
        Canvas {
            id: priceCanvas
            objectName: "priceCanvas"
            width: internal.priceAxisWidth
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            z: 2

            Component.onCompleted: console.log("priceCanvas ready", width, height)

            function drawPriceAxis(ctx) {
                ctx.fillStyle = "#8a8a8a";
                ctx.font = "10px sans-serif";
                ctx.textBaseline = "middle";
            }

            function drawDivisionsPriceAxis(ctx) {
                var priceRange = internal.maxPrice - internal.minPrice;
                var step = internal.calculateGridStep(priceRange, 5);
                var firstPrice = Math.ceil(internal.minPrice / step) * step;
                for (var price = firstPrice; price <= internal.maxPrice; price += step) {
                    var yPos = Math.round(internal.convertPriceToY(price, height));
                    var decimalPlaces = step < 1 ? 2 : (step % 1 === 0 ? 0 : 1);
                    ctx.fillText(price.toFixed(decimalPlaces), 8, yPos);
                }
            }

            function drawPriceOpenCandle(ctx) {
                var lastCandle = klineSeries.last()

                if (!internal.isValid(lastCandle))
                    return

                if (lastCandle && !lastCandle.isConfirm) {
                    var curPriceY = internal.convertPriceToY(lastCandle.close, height);
                    var isUp = lastCandle.close > lastCandle.open;

                    var priceText = lastCandle.close.toFixed(2);
                    ctx.font = "bold 10px sans-serif";
                    var textWidth = ctx.measureText(priceText).width + 10;
                    var textHeight = 16;

                    var textX = 2;
                    var textY = curPriceY - textHeight / 2;

                    if (textY < 0)
                        textY = 0;
                    if (textY + textHeight > height)
                        textY = height - textHeight;

                    ctx.fillStyle = isUp ? "rgba(102, 187, 106, 0.85)" : "rgba(239, 83, 80, 0.85)";
                    ctx.strokeStyle = isUp ? "rgba(102, 187, 106, 0.5)" : "rgba(239, 83, 80, 0.5)";
                    ctx.lineWidth = 1;

                    var radius = 3;
                    ctx.beginPath();
                    ctx.moveTo(textX + radius, textY);
                    ctx.lineTo(textX + textWidth - radius, textY);
                    ctx.quadraticCurveTo(textX + textWidth, textY, textX + textWidth, textY + radius);
                    ctx.lineTo(textX + textWidth, textY + textHeight - radius);
                    ctx.quadraticCurveTo(textX + textWidth, textY + textHeight, textX + textWidth - radius, textY + textHeight);
                    ctx.lineTo(textX + radius, textY + textHeight);
                    ctx.quadraticCurveTo(textX, textY + textHeight, textX, textY + textHeight - radius);
                    ctx.lineTo(textX, textY + radius);
                    ctx.quadraticCurveTo(textX, textY, textX + radius, textY);
                    ctx.closePath();
                    ctx.fill();
                    ctx.stroke();

                    ctx.fillStyle = "#ffffff";
                    ctx.font = "bold 10px sans-serif";
                    ctx.textBaseline = "middle";
                    ctx.fillText(priceText, textX + 5, textY + textHeight / 2);

                    ctx.strokeStyle = isUp ? "rgba(102, 187, 106, 0.5)" : "rgba(239, 83, 80, 0.5)";
                    ctx.lineWidth = 1;
                    ctx.setLineDash([2, 2]);
                    ctx.beginPath();
                    ctx.moveTo(0, curPriceY);
                    ctx.lineTo(width, curPriceY);
                    ctx.stroke();
                    ctx.setLineDash([]);
                }
            }

            Connections {
                target: internal
                function onMouseGlobalYChanged() {
                    internal.safeRequestPaint(priceCanvas)
                }
                function onMouseInsideChanged() {
                   internal.safeRequestPaint(priceCanvas)
                }
                function onPriceAtCursorChanged() {
                   internal.safeRequestPaint(priceCanvas)
                }
            }

            onPaint: {
                //console.log("onPaint priceCanvas:", width, height)

                if (width === 0 || height === 0)
                    return;

                var ctx = getContext("2d")
                ctx.clearRect(0, 0, width, height);
                ctx.save()
                ctx.scale(1, 1)

                drawPriceAxis(ctx)
                drawDivisionsPriceAxis(ctx)
                if (root.enableDisplayOpenPrice)
                    drawPriceOpenCandle(ctx)

                ctx.restore()
            }
        } // priceCanvas

        Rectangle {
            width: 1
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.right: priceCanvas.left
            color: "#3a3a3a"
        }

        // Ценовой график
        Flickable {
            id: priceChartScrollView
            anchors.left: parent.left
            anchors.right: priceCanvas.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom

            flickableDirection: Flickable.HorizontalFlick
            contentWidth: Math.max(parent.width, internal.totalChartWidth)
            contentHeight: height
            boundsBehavior: Flickable.StopAtBounds
            interactive: true
            clip: true

            onContentXChanged: {
                if (!internal.updatingScroll) {
                    internal.updatingScroll = true
                    var clampedX = Math.max(0, contentX)
                    if (volumeChartScrollView.contentX !== clampedX)
                        volumeChartScrollView.contentX = clampedX
                    internal.updatingScroll = false
                }

                internal.updateScrollPosition()
                internal.updateVisibleRange()

                if (contentX <= 0)
                    root.leftBoundaryReached()
            }

            onWidthChanged: {
                if (internal.updatingScroll)
                    return

                priceChartScrollView.contentWidth = Math.max(priceChartScrollView.parent.width, internal.totalChartWidth);

                if (internal.autoScrollEnabled && !internal.isUserInteracting) {
                    internal.scrollToLastCandle(false)
                    return
                }

                if (internal.totalChartWidth <= width) {
                    priceChartScrollView.contentX = 0;
                    internal.scrollPosition = 0;
                } else {
                    var maxScrollX = Math.max(0, priceChartScrollView.contentWidth - priceChartScrollView.width)
                    priceChartScrollView.contentX = Math.max(0, Math.min(internal.scrollPosition * maxScrollX, maxScrollX))
                }

                internal.syncScrollViews()
                internal.updateVisibleRange()
            }

            onMovementStarted: {
                if (internal.totalChartWidth > width) {
                    internal.isUserInteracting = true;
                    internal.autoScrollEnabled = false;
                    internal.isScrolling = true;
                    autoScrollRestartTimer.start();
                    internal.updateCrosshair()
                    internal.updateAxes()
                }
            }

            onMovementEnded: {
                if (internal.totalChartWidth > width) {
                    internal.isUserInteracting = false;
                    internal.isScrolling = false;
                    internal.syncScrollViews()
                    internal.updateScrollPosition()
                    internal.updateCrosshair()
                    internal.updateAxes()
                }
            }

            Canvas {
                id: priceChartCanvas
                objectName: "priceChartCanvas"
                width: priceChartScrollView.contentWidth
                height: priceChartScrollView.contentHeight

                Component.onCompleted: console.log("priceChartCanvas ready", width, height)

                function drawGridChart(ctx) {
                    if (!root.klineSeries)
                        return

                    ctx.save()
                    ctx.resetTransform()

                    var priceRange = internal.maxPrice - internal.minPrice
                    var step = internal.calculateGridStep(priceRange, 5)
                    var firstPrice = Math.ceil(internal.minPrice / step) * step

                    ctx.strokeStyle = "#2d2d2d"
                    ctx.lineWidth = 1

                    for (var price = firstPrice; price <= internal.maxPrice; price += step) {
                        var yGrid = Math.round(internal.convertPriceToY(price, height))

                        ctx.beginPath()
                        ctx.moveTo(0, yGrid)
                        ctx.lineTo(width, yGrid)
                        ctx.stroke()
                    }

                    ctx.restore()
                }

                function drawCandle(ctx, xPos, candle) {
                    var yHigh = internal.convertPriceToY(candle.high, height);
                    var yLow = internal.convertPriceToY(candle.low, height);
                    var yOpen = internal.convertPriceToY(candle.open, height);
                    var yClose = internal.convertPriceToY(candle.close, height);

                    var color;
                    if (!candle.isConfirm)
                        color = candle.close > candle.open ? "#66BB6A" : "#EF5350";
                    else
                        color = candle.close > candle.open ? "#00C853" : "#FF5252";

                    ctx.fillStyle = color;
                    ctx.strokeStyle = color;

                    ctx.beginPath();
                    ctx.moveTo(xPos + internal.candleWidth / 2 + 0.5, yHigh);
                    ctx.lineTo(xPos + internal.candleWidth / 2 + 0.5, yLow);
                    ctx.stroke();

                    var bodyY = Math.min(yOpen, yClose);
                    var bodyHeight = Math.max(Math.abs(yOpen - yClose), 1);
                    ctx.fillRect(xPos, bodyY, internal.candleWidth, bodyHeight);
                }

                function drawCandles(ctx)
                {
                    if (!root.klineSeries || root.klineSeries.length === 0)
                        return

                    var startIdx = internal.firstVisibleIdx;
                    var endIdx = internal.lastVisibleIdx;
                    if (startIdx < 0 || endIdx < 0)
                    {
                        startIdx = 0;
                        endIdx = root.klineSeries.length - 1;
                    }

                    var step = internal.candleWidth + internal.candleSpacing
                    for (var i = startIdx; i <= endIdx; i++)
                    {
                        var candle = root.klineSeries.get(i)

                        if (!internal.isValid(candle))
                            continue

                        var xPos = i * step + internal.candleSpacing / 2

                        if (i % 5 === 0)
                        {
                            ctx.strokeStyle = "#252525"
                            ctx.beginPath()
                            ctx.moveTo(xPos + internal.candleWidth / 2, 0)
                            ctx.lineTo(xPos + internal.candleWidth / 2, height)
                            ctx.stroke()
                        }

                        drawCandle(ctx, xPos, candle)
                    }
                }

                function drawLineCurrentPrice(ctx)
                {
                    var lastCandle = klineSeries.last()

                    if (!internal.isValid(lastCandle))
                        return

                    if (lastCandle && !lastCandle.isConfirm)
                    {
                        var curPriceY = internal.convertPriceToY(lastCandle.close, height)
                        var isUp = lastCandle.close > lastCandle.open

                        ctx.save()
                        ctx.resetTransform()

                        ctx.strokeStyle = isUp ? "rgba(102, 187, 106, 0.7)" : "rgba(239, 83, 80, 0.7)"
                        ctx.lineWidth = 1
                        ctx.setLineDash([3, 3])
                        ctx.beginPath()
                        ctx.moveTo(0, curPriceY)
                        ctx.lineTo(width, curPriceY)
                        ctx.stroke()
                        ctx.setLineDash([])

                        ctx.restore()
                    }
                }

                onPaint: {
                    //console.log("onPaint priceChartCanvas:", width, height)

                    if (width === 0 || height === 0)
                        return

                    var ctx = getContext("2d")
                    ctx.clearRect(0, 0, width, height);
                    ctx.save();

                    drawGridChart(ctx)
                    drawCandles(ctx)
                    if (root.enableDisplayOpenPrice)
                        drawLineCurrentPrice(ctx)

                    ctx.restore()
                }
            } // priceChartCanvas
        } // priceChartScrollView
    } // priceArea

    // Разделитель между ценовым графиком и объемами
    Rectangle {
        id: priceVolumeSeparator
        height: root.enableShowVolumes ? 2 : 0
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: priceArea.bottom
        color: "#3a3a3a"
        z: 5
        visible: root.enableShowVolumes
    } // priceVolumeSeparator

    // Область графика объемов (нижняя часть)
    Rectangle {
        id: volumeArea
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: priceVolumeSeparator.bottom
        height: root.enableShowVolumes ? (parent.height - internal.timeAxisHeight) * root.volumeChartHeightRatio : 0
        visible: root.enableShowVolumes
        color: "transparent"

        // Обработчики изменения размеров для принудительной перерисовки
        onWidthChanged: {
            if (width === 0 || height === 0)
                return;
            internal.safeRequestPaint(volumeCanvas)
            internal.safeRequestPaint(volumeChartCanvas)
        }
        onHeightChanged: {
            if (width === 0 || height === 0)
                return;
            internal.safeRequestPaint(volumeCanvas)
            internal.safeRequestPaint(volumeChartCanvas)
        }

        // Ценовая информация о свече
        Rectangle {
            anchors.left: parent.left
            anchors.top: parent.top
            width: volumeInfoLayout.width + 16
            height: volumeInfoLayout.height + 16
            color: "transparent"
            enabled: root.enableCursorInfo
            z: 20

            Row {
                id: volumeInfoLayout
                anchors.centerIn: parent
                spacing: 10
                padding: 2

                Text {
                    text: "V:"
                    color: "#8a8a8a"
                    font.pixelSize: 11
                    font.family: "monospace"
                }
                Text {
                    text: internal.volumeHoveredCandle
                    color: internal.colorHoveredCandle
                    font.pixelSize: 11
                    font.family: "monospace"
                }

                Text {
                    text: "T:"
                    color: "#8a8a8a"
                    font.pixelSize: 11
                    font.family: "monospace"
                }
                Text {
                    text: internal.turnoverHoveredCandle
                    color: internal.colorHoveredCandle
                    font.pixelSize: 11
                    font.family: "monospace"
                }
            } // priceInfoLayout
        }

        // Правая ось для объемов
        Canvas {
            id: volumeCanvas
            objectName: "volumeCanvas"
            width: internal.priceAxisWidth
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            visible: root.enableShowVolumes
            z: 2

            Component.onCompleted: console.log("volumeCanvas ready", width, height)

            function drawVolumeAxis(ctx) {
                ctx.fillStyle = "#8a8a8a";
                ctx.font = "10px sans-serif";
                ctx.textBaseline = "middle";
            }

            function drawDivisionsVolumeAxis(ctx) {
                if (internal.maxVolume === 0)
                    return;

                var targetLines = 3;
                var step = internal.calculateGridStep(internal.maxVolume, targetLines);
                var firstValue = Math.ceil(0 / step) * step;

                for (var vol = firstValue; vol <= internal.maxVolume; vol += step) {
                    if (vol === 0)
                        continue;
                    var yPos = height - (vol / internal.maxVolume) * height;
                    var text = vol >= 1000000 ? (vol / 1000000).toFixed(2) + "M" :
                              vol >= 1000 ? (vol / 1000).toFixed(2) + "K" :
                              vol.toFixed(2);
                    ctx.fillText(text, /*width - 5*/
                                 8, yPos);
                }
            }

            Connections {
                target: internal
                function onMouseGlobalXChanged() {
                    internal.safeRequestPaint(volumeCanvas)
                }
                function onMouseInsideChanged() {
                    internal.safeRequestPaint(volumeCanvas)
                }
            }

            onPaint: {
                //console.log("onPaint volumeCanvas:", width, height)

                if (width === 0 || height === 0)
                    return

                var ctx = getContext("2d")
                ctx.clearRect(0, 0, width, height);
                ctx.save()

                drawVolumeAxis(ctx)
                drawDivisionsVolumeAxis(ctx)

                ctx.restore()
            }
        } // volumeCanvas

        Rectangle {
            width: 1
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.right: volumeCanvas.left
            color: "#3a3a3a"
        }

        // График объемов
        Flickable {
            id: volumeChartScrollView
            anchors.left: parent.left
            anchors.right: volumeCanvas.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            visible: root.enableShowVolumes
            clip: true
            interactive: true
            boundsBehavior: Flickable.StopAtBounds

            contentWidth: priceChartScrollView.contentWidth
            contentHeight: height

            onContentXChanged: {
                if (!internal.updatingScroll) {
                    internal.updatingScroll = true
                    var clampedX = Math.max(0, contentX)
                    if (priceChartScrollView.contentX !== clampedX)
                        priceChartScrollView.contentX = clampedX
                    internal.updatingScroll = false
                }
                internal.updateScrollPosition()
            }

            Canvas {
                id: volumeChartCanvas
                objectName: "volumeChartCanvas"
                width: volumeChartScrollView.contentWidth
                height: volumeChartScrollView.contentHeight

                Component.onCompleted: console.log("volumeChartCanvas ready", width, height)

                function drawVolumes(ctx)
                {
                    if (!root.klineSeries || root.klineSeries.length === 0 || internal.maxVolume === 0)
                        return;

                    var startIdx = internal.firstVisibleIdx;
                    var endIdx = internal.lastVisibleIdx;
                    if (startIdx < 0 || endIdx < 0)
                    {
                        startIdx = 0;
                        endIdx = root.klineSeries.length - 1;
                    }

                    var step = internal.candleWidth + internal.candleSpacing;
                    var totalHeight = height;

                    for (var i = startIdx; i <= endIdx; i++)
                    {
                        var candle = root.klineSeries.get(i);

                        if (!internal.isValid(candle))
                            continue;

                        var normalizedVolume = candle.volume / internal.maxVolume;
                        var barHeight = normalizedVolume * totalHeight;
                        var xPos = i * step + internal.candleSpacing / 2;

                        var color;
                        if (!candle.isConfirm)
                            color = candle.close > candle.open ? "#66BB6A" : "#EF5350";
                        else
                            color = candle.close > candle.open ? "#00C853" : "#FF5252";

                        ctx.fillStyle = color;
                        ctx.fillRect(xPos, totalHeight - barHeight, internal.candleWidth, barHeight);

                        if (i % 5 === 0)
                        {
                            ctx.strokeStyle = "#252525"
                            ctx.beginPath()
                            ctx.moveTo(xPos + internal.candleWidth / 2, 0)
                            ctx.lineTo(xPos + internal.candleWidth / 2, totalHeight)
                            ctx.stroke()
                        }
                    }
                }

                function drawVolumeHorizontalLines(ctx)
                {
                    if (internal.maxVolume === 0)
                        return;

                    ctx.save()
                    ctx.resetTransform()

                    var targetLines = 3;
                    var step = internal.calculateGridStep(internal.maxVolume, targetLines);
                    var firstValue = Math.ceil(0 / step) * step;

                    ctx.strokeStyle = "#2d2d2d";
                    ctx.lineWidth = 1;

                    for (var vol = firstValue; vol <= internal.maxVolume; vol += step)
                    {
                        if (vol === 0)
                            continue;
                        var yPos = height - (vol / internal.maxVolume) * height;
                        ctx.beginPath();
                        ctx.moveTo(0, yPos);
                        ctx.lineTo(width, yPos);
                        ctx.stroke();
                    }

                    ctx.restore()
                }

                onPaint: {
                    //console.log("onPaint volumeChartCanvas:", width, height)

                    if (width === 0 || height === 0)
                        return

                    var ctx = getContext("2d");
                    ctx.clearRect(0, 0, width, height);
                    ctx.save();

                    drawVolumeHorizontalLines(ctx);
                    drawVolumes(ctx);

                    ctx.restore();
                }
            } // volumeChartCanvas
        } // volumeChartScrollView
    } // volumeArea

    // Ось времени (всегда внизу)
    Rectangle {
        id: timeAxisArea
        height: internal.timeAxisHeight
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        color: "#1e1e1e"
        border.width: 1
        border.color: "#3a3a3a"
        clip: true
        z: 2

        Canvas {
            id: timeCanvas
            objectName: "timeCanvas"
            width: priceChartScrollView.contentWidth
            height: parent.height
            x: -priceChartScrollView.contentX

            Component.onCompleted: console.log("timeCanvas ready", width, height)

            function drawTimeAxis(ctx)
            {
                if (!root.klineSeries)
                    return;

                ctx.fillStyle = "#8a8a8a";
                ctx.font = "10px sans-serif";
                ctx.textAlign = "center";
            }

            function drawDivisionsTimeAxis(ctx)
            {
                var step = internal.candleWidth + internal.candleSpacing;
                var offsetX = -x;
                var containerWidth = width;

                var visibleStart = Math.max(0, Math.floor(offsetX / step));
                var visibleEnd = Math.min(root.klineSeries.length - 1, Math.ceil((offsetX + containerWidth) / step));

                visibleStart = Math.max(0, visibleStart - 1);
                visibleEnd = Math.min(root.klineSeries.length - 1, visibleEnd + 1);

                var labelStep = 5
                if (internal.candleWidth < 8)
                    labelStep = 20
                else if (internal.candleWidth < 15)
                    labelStep = 10
                else if (internal.candleWidth > 40)
                    labelStep = 2
                else if (internal.candleWidth > 60)
                    labelStep = 1

                for (var i = visibleStart; i <= visibleEnd; i++)
                {
                    if (i % labelStep === 0)
                    {
                        var candle = root.klineSeries.get(i)
                        if (!internal.isValid(candle))
                            continue

                        var xPos = i * step + internal.candleSpacing / 2
                        var timeStr = Qt.formatDateTime(new Date(candle.time), "HH:mm")
                        ctx.fillText(timeStr, xPos + internal.candleWidth / 2, height / 2 + 3)
                    }
                }
            }

            Connections {
                target: priceChartScrollView
                function onContentXChanged() {
                    internal.safeRequestPaint(timeCanvas)
                }
                function onWidthChanged() {
                    internal.safeRequestPaint(timeCanvas)
                }
            }

            Connections {
                target: internal
                function onMouseGlobalXChanged() {
                    internal.safeRequestPaint(timeCanvas)
                }
                function onMouseInsideChanged() {
                    internal.safeRequestPaint(timeCanvas)
                }
                function onTimeAtCursorChanged() {
                    internal.safeRequestPaint(timeCanvas)
                }
            }

            onPaint: {
                if (width === 0 || height === 0)
                    return

                var ctx = getContext("2d")
                ctx.clearRect(0, 0, width, height)

                ctx.save()
                ctx.scale(1,1)

                drawTimeAxis(ctx)
                drawDivisionsTimeAxis(ctx)

                ctx.restore()
            }
        } // timeCanvas
    } // timeAxisArea

    Connections {
        target: klineSeries

        function onModelUpdated()
        {
            console.log("onModelUpdated")
            internal.scrollPosition = 0;
            Qt.callLater(function() {

                if (internal.hoveredCandleIndex >= 0) {
                    var idx = internal.hoveredCandleIndex
                    internal.hoveredCandleIndex = -1
                    internal.hoveredCandleIndex = idx
                }

                internal.update()
            });
        }
    }

    Component.onCompleted: {
        if (root.klineSeries && root.klineSeries.length > 0) {
            Qt.callLater(function() {
                console.log("Root component ready:", width, height)
                internal.update()
            });
        }
    }

}