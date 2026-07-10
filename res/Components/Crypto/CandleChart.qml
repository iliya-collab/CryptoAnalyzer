import QtQuick 2.15

Rectangle {
    id: root

    color: "#1e1e1e"
    border.width: 1
    border.color: "#3a3a3a"

    /*Rectangle {
        id: statsIndicator
        anchors.bottom: timeAxisContainer.top
        anchors.bottomMargin: 5
        anchors.right: priceCanvas.left
        anchors.rightMargin: 10
        color: Qt.rgba(0, 0, 0, 0.7)
        radius: 4
        visible: root.candleSeries && root.candleSeries.length > 0
        z: 20

        Column {
            spacing: 10
            Text {
                color: "#8a8a8a"
                font.pixelSize: 10
                text: "Всего: " + root.candleSeries.length
            }
            Text {
                color: "#8a8a8a"
                font.pixelSize: 10
                text: {
                    var visible = internal.getVisibleCandleIndices()
                    if (visible.start >= 0 && visible.end >= 0)
                        return "Видно: " + (visible.end - visible.start + 1)
                    return ""
                }
            }
        }
    }*/

    QtObject {
        id: internal

        // Ширина свечи
        property int candleWidth: 20
        // Оступы между свечой
        property int candleSpacing: 6
        // Ширина оси цен
        property int priceAxisWidth: 50
        // Высота оси времени
        property int timeAxisHeight: 40
        // Общая ширина графика
        property int totalChartWidth: candleSeries ? candleSeries.length * (candleWidth + candleSpacing) : 0
        // Флаг автоматической прокрутки к последней свече
        property bool autoScrollEnabled: true
        // Флаг, показывающий, что идет ручная прокрутка
        property bool isUserInteracting: false
        // Флаг скроллинга
        property bool isScrolling: false

        // Настройки зума
        property int minCandleWidth: 2
        property int maxCandleWidth: 80
        property int zoomStep: 2

        // Сохраняем позицию
        property real scrollPosition: 0.0
        property bool updatingScroll: false
        property int firstVisibleIdx: -1
        property int lastVisibleIdx: -1

        // Свойства для кросс-курсора
        property real mouseGlobalX: -1
        property real mouseGlobalY: -1
        property real mouseLocalX: -1
        property real mouseLocalY: -1
        property bool mouseInside: false
        property int hoveredCandleIndex: -1
        property real priceAtCursor: 0
        property string timeAtCursor: ""

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
            if (!root.candleSeries || root.candleSeries.length === 0)
                return { start: -1, end: -1 }

            var step = internal.candleWidth + internal.candleSpacing

            var buffer = 2
            var firstIdx = Math.max(0, Math.floor(chartScrollView.contentX / step) - buffer)
            var lastIdx = Math.min(root.candleSeries.length - 1,
                                   Math.ceil((chartScrollView.contentX + chartScrollView.width) / step) + buffer)

            return { start: firstIdx, end: lastIdx }
        }

        // Функция для проверки, видна ли свеча
        function isCandleVisible(index) {
            if (!root.candleSeries || index < 0 || index >= root.candleSeries.length)
                return false

            var step = internal.candleWidth + internal.candleSpacing

            var candleX = index * step + internal.candleSpacing / 2
            var candleEndX = candleX + internal.candleWidth

            // Проверяем, пересекается ли свеча с видимой областью
            return (candleX < chartScrollView.contentX + chartScrollView.width &&
                    candleEndX > chartScrollView.contentX)
        }

        function updateVisibleRange() {
            if (scrollAnimation.running)
                return;
            if (!root.candleSeries || root.candleSeries.length === 0)
                return;
            if (chartScrollView.width <= 0)
                return;

            var step = internal.candleWidth + internal.candleSpacing;

            // Определяем видимые свечи
            var visibleIdx = getVisibleCandleIndices()
            internal.firstVisibleIdx = visibleIdx.start
            internal.lastVisibleIdx = visibleIdx.end

            // Находим min/max только среди видимых свечей
            var currentMax = -Infinity;
            var currentMin = Infinity;

            for (var i = firstVisibleIdx; i <= lastVisibleIdx; i++) {
                var candle = root.candleSeries[i];
                if (candle) {
                    if (candle.high > currentMax)
                        currentMax = candle.high;
                    if (candle.low < currentMin)
                        currentMin = candle.low;
                }
            }

            if (currentMax !== -Infinity && currentMin !== Infinity && currentMax >= currentMin) {
                var priceRange = currentMax - currentMin;
                var padding = priceRange * 0.15;
                if (padding === 0)
                    padding = 1.0;

                root.maxPrice = currentMax + padding;
                root.minPrice = currentMin - padding;

                chartCanvas.requestPaint();
                priceCanvas.requestPaint();
            }
        }

        function scrollToLastCandle(animated) {
            if (!root.candleSeries || root.candleSeries.length === 0)
                return;

            var maxScrollX = Math.max(0, chartScrollView.contentWidth - chartScrollView.width);

            if (maxScrollX <= 0) {
                chartScrollView.contentX = 0
                return
            }

            if (animated) {
                scrollAnimation.to = maxScrollX;
                scrollAnimation.start();
            } else {
                chartScrollView.contentX = maxScrollX;
                Qt.callLater(function() {
                    timeCanvas.requestPaint();
                    internal.updateCrosshair();
                });
            }
        }

        function getCandleAtX(x) {
            if (!root.candleSeries || root.candleSeries.length === 0)
                return null;

            var step = internal.candleWidth + internal.candleSpacing

            //console.log("offsetX:", offsetX, "mouseX:", x, "step:", step)

            // Вычисляем индекс предполагаемой свечи
            var index = Math.floor((x - internal.candleSpacing / 2) / step)

            //console.log("mouseX:", x)
            //console.log("index:", index)

            if (index < 0 || index >= root.candleSeries.length)
                return null;

            // Координаты начала и конца текущей свечи
            var candleStartX = index * step + internal.candleSpacing / 2
            var candleEndX = candleStartX + internal.candleWidth

            // Если курсор не попадает в этот диапазон – значит, он между свечами
            if (x < candleStartX || x > candleEndX)
                return null;


            return { index: index };
        }

        function getPriceY(price, availableHeight) {
            var range = root.maxPrice - root.minPrice
            if (range === 0)
                return availableHeight / 2;
            return availableHeight - ((price - root.minPrice) / range) * availableHeight;
        }

        function getPriceAtY(y, availableHeight) {
            var range = root.maxPrice - root.minPrice;
            if (range === 0)
                return root.maxPrice;
            return root.maxPrice - (y / availableHeight) * range;
        }

        function getTimeAtX(x) {
            if (!root.candleSeries || root.candleSeries.length === 0)
                return -1
            var result = getCandleAtX(x)
            if (result && result.index >= 0 && result.index < root.candleSeries.length) {
                var candle = root.candleSeries[result.index];
                if (!candle)
                    return -1
                return (candle.end !== undefined) ? candle.end : -1
            }
            return -1
        }

        function updateCrosshair() {
            if (!internal.mouseInside || internal.mouseGlobalX < 0 || internal.mouseGlobalY < 0) {
                crosshairCanvas.requestPaint();
                priceCanvas.requestPaint();
                timeCanvas.requestPaint();
                return;
            }

            // Обновляем данные о свече под курсором
            var result = internal.getCandleAtX(internal.mouseGlobalX);
            if (result && result.index >= 0 && result.index < root.candleSeries.length) {
                internal.hoveredCandleIndex = result.index;
                var candle = root.candleSeries[result.index];
            } else
                internal.hoveredCandleIndex = -1;


            var timestamp = internal.getTimeAtX(internal.mouseGlobalX)
            //internal.timeAtCursor = (timestamp > 0) ? Qt.formatDateTime(new Date(timestamp * 1000), "dd.MM.yyyy HH:mm:ss") : ""
            internal.timeAtCursor = (timestamp > 0) ? Qt.formatDateTime(new Date(timestamp), "dd.MM.yyyy HH:mm") : ""
            internal.priceAtCursor = internal.getPriceAtY(internal.mouseGlobalY, chartScrollView.height)

            // Принудительно перерисовываем кросс-курсор
            crosshairCanvas.requestPaint();
            priceCanvas.requestPaint();
            timeCanvas.requestPaint();
        }

        function saveScrollPosition() {
            var maxScrollX = Math.max(1, chartScrollView.contentWidth - chartScrollView.width);
            if (maxScrollX > 0) {
                internal.scrollPosition = chartScrollView.contentX / maxScrollX;
                internal.scrollPosition = Math.max(0, Math.min(1, internal.scrollPosition));
            } else {
                internal.scrollPosition = 0;
            }
        }

        function restoreScrollPosition(animated) {
            var maxScrollX = Math.max(0, chartScrollView.contentWidth - chartScrollView.width)
            var targetX = internal.scrollPosition * maxScrollX
            targetX = Math.max(0, Math.min(targetX, maxScrollX))

            if (animated) {
                scrollAnimation.to = targetX
                scrollAnimation.start()
            } else
                chartScrollView.contentX = targetX
        }

        function updateScrollPosition() {
            var maxScrollX = Math.max(1, chartScrollView.contentWidth - chartScrollView.width);
            if (!internal.updatingScroll && maxScrollX > 0) {
                internal.scrollPosition = chartScrollView.contentX / maxScrollX;
                internal.scrollPosition = Math.max(0, Math.min(1, internal.scrollPosition));
            }
        }

        function updateChart() {
            Qt.callLater(function() {
                if (!root.candleSeries || root.candleSeries.length === 0) {
                    chartCanvas.requestPaint()
                    priceCanvas.requestPaint()
                    timeCanvas.requestPaint()
                    return
                }

                chartScrollView.contentWidth = Math.max(root.width, internal.totalChartWidth)

                internal.updateVisibleRange()

                chartCanvas.requestPaint()
                priceCanvas.requestPaint()
                timeCanvas.requestPaint()
                internal.updateCrosshair()
            })
        }

        function getLastCandle() {
            var lastIdx = root.candleSeries.length - 1

            if (lastIdx < 0)
                return null;

            return root.candleSeries[lastIdx]
        }
    }

    // Набор свечей
    property var candleSeries: null
    // Максимальная цена на оси цен
    property real maxPrice: 100
    // Минимальная цена на оси цен
    property real minPrice: 0
    property bool enableAutoScroll: true
    property bool enableCursorTime: true
    property bool enableCursorPrice: true
    property bool enableCursorInfo: true
    property bool enableDisplayOpenPrice: true
    property int timeAutoScroll: 3000

    signal leftBoundaryReached()

    onCandleSeriesChanged: {
        internal.updateChart()
    }

    function zoomIn() {
        var newWidth = Math.min(internal.candleWidth + internal.zoomStep, internal.maxCandleWidth);

        if (newWidth !== internal.candleWidth) {
            internal.saveScrollPosition()
            internal.updatingScroll = true

            internal.candleWidth = newWidth
            chartScrollView.contentWidth = Math.max(root.width, internal.totalChartWidth)

            if (internal.totalChartWidth <= chartScrollView.width) {
                chartScrollView.contentX = 0
                internal.scrollPosition = 0
            } else
                internal.restoreScrollPosition(false)


            internal.updatingScroll = false

            internal.updateVisibleRange()
            chartCanvas.requestPaint()
            priceCanvas.requestPaint()
            timeCanvas.requestPaint()
            internal.updateCrosshair()
        }
    }

    function zoomOut() {
        var newWidth = Math.max(internal.candleWidth - internal.zoomStep, internal.minCandleWidth);

        if (newWidth !== internal.candleWidth) {
            internal.saveScrollPosition();
            internal.updatingScroll = true;

            internal.candleWidth = newWidth;
            chartScrollView.contentWidth = Math.max(root.width, internal.totalChartWidth);

            if (internal.totalChartWidth <= chartScrollView.width) {
                chartScrollView.contentX = 0;
                internal.scrollPosition = 0;
            } else {
                internal.restoreScrollPosition(false);
            }

            internal.updatingScroll = false;

            internal.updateVisibleRange();
            chartCanvas.requestPaint();
            priceCanvas.requestPaint();
            timeCanvas.requestPaint();
            internal.updateCrosshair();
        }
    }

    function resetZoom() {
        internal.saveScrollPosition();
        internal.updatingScroll = true;

        internal.candleWidth = 20;
        chartScrollView.contentWidth = Math.max(root.width, internal.totalChartWidth);

        if (internal.totalChartWidth <= chartScrollView.width) {
            chartScrollView.contentX = 0;
            internal.scrollPosition = 0;
        } else {
            internal.restoreScrollPosition(true);
        }

        internal.updatingScroll = false;

        internal.updateVisibleRange();
        chartCanvas.requestPaint();
        priceCanvas.requestPaint();
        timeCanvas.requestPaint();
        internal.updateCrosshair();
    }

    NumberAnimation {
        id: scrollAnimation
        target: chartScrollView
        property: "contentX"
        duration: 200
        easing.type: Easing.OutCubic
        onStopped: {
            internal.updateVisibleRange();
            internal.updateScrollPosition();
            internal.updateCrosshair();
        }
    }

    // Фиксированная ось цены Oy (справа)
    Canvas {
        id: priceCanvas
        width: internal.priceAxisWidth
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: timeAxisContainer.top
        z: 2

        function drawPriceAxis(ctx) {
            ctx.fillStyle = "#8a8a8a";
            ctx.font = "10px sans-serif";
            ctx.textBaseline = "middle";
        }

        function drawDivisionsPriceAxis(ctx) {
            var priceRange = root.maxPrice - root.minPrice;
            var step = internal.calculateGridStep(priceRange, 5);
            var firstPrice = Math.ceil(root.minPrice / step) * step;
            for (var price = firstPrice; price <= root.maxPrice; price += step) {
                var yPos = Math.round(internal.getPriceY(price, height));
                var decimalPlaces = step < 1 ? 2 : (step % 1 === 0 ? 0 : 1);
                ctx.fillText(price.toFixed(decimalPlaces), 8, yPos);
            }
        }

        function drawCursorPrice(ctx) {
            if (internal.mouseInside && internal.mouseGlobalX >= 0 && internal.mouseGlobalY <= height) {
                var cursorY = internal.mouseGlobalY;

                // Фон для текста цены
                var priceText = internal.priceAtCursor.toFixed(2);
                ctx.font = "bold 10px sans-serif";
                var textWidth = ctx.measureText(priceText).width + 10;
                var textHeight = 16;

                // Позиционируем текст справа от оси
                var textX = width - textWidth - 2;
                var textY = cursorY - textHeight / 2;

                // Корректируем, чтобы не выходил за границы
                if (textY < 0)
                    textY = 0;
                if (textY + textHeight > height)
                    textY = height - textHeight;

                // Рисуем фон
                ctx.fillStyle = "rgba(30, 30, 30, 0.85)";
                ctx.strokeStyle = "rgba(255, 255, 255, 0.2)";
                ctx.lineWidth = 1;

                // Закругленный прямоугольник
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

                // Рисуем текст с ценой
                ctx.fillStyle = "#ffffff";
                ctx.font = "bold 10px sans-serif";
                ctx.textBaseline = "middle";
                ctx.fillText(priceText, textX + 5, textY + textHeight / 2);

                // Маленький индикатор на оси
                ctx.strokeStyle = "rgba(255, 255, 255, 0.3)";
                ctx.lineWidth = 1;
                ctx.setLineDash([2, 2]);
                ctx.beginPath();
                ctx.moveTo(0, cursorY);
                ctx.lineTo(width, cursorY);
                ctx.stroke();
                ctx.setLineDash([]);
            }
        }

        function drawPriceOpenCandle(ctx) {
            var lastCandle = internal.getLastCandle()
            if (lastCandle && !lastCandle.isConfirm) {

                var curPriceY = internal.getPriceY(lastCandle.close, height);
                var isUp = lastCandle.close > lastCandle.open;

                var priceText = lastCandle.close.toFixed(2);

                ctx.font = "bold 10px sans-serif";
                var textWidth = ctx.measureText(priceText).width + 10;
                var textHeight = 16;

                // Позиционируем текст слева от оси
                var textX = 2;
                var textY = curPriceY - textHeight / 2;

                // Корректируем, чтобы не выходил за границы
                if (textY < 0)
                    textY = 0;
                if (textY + textHeight > height)
                    textY = height - textHeight;

                // Рисуем фон с цветом свечи
                ctx.fillStyle = isUp ? "rgba(102, 187, 106, 0.85)" : "rgba(239, 83, 80, 0.85)";
                ctx.strokeStyle = isUp ? "rgba(102, 187, 106, 0.5)" : "rgba(239, 83, 80, 0.5)";
                ctx.lineWidth = 1;

                // Закругленный прямоугольник
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

                // Рисуем текст с ценой
                ctx.fillStyle = "#ffffff";
                ctx.font = "bold 10px sans-serif";
                ctx.textBaseline = "middle";
                ctx.fillText(priceText, textX + 5, textY + textHeight / 2);

                // Маленький индикатор на оси
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
               priceCanvas.requestPaint();
            }
            function onMouseInsideChanged() {
               priceCanvas.requestPaint();
            }
            function onPriceAtCursorChanged() {
               priceCanvas.requestPaint();
            }
        }

        onPaint: {
            var ctx = getContext("2d")
            ctx.clearRect(0, 0, width, height);

            ctx.save()

            ctx.scale(1, 1)

            // Рисуем ось Oy
            drawPriceAxis(ctx)

            // Рисуем деления на оси Oy
            drawDivisionsPriceAxis(ctx)

            // Рисуем текущую цену курсора
            if (root.enableCursorPrice)
                drawCursorPrice(ctx)

            // Рисуем цену открытой свечи
            if (root.enableDisplayOpenPrice)
                drawPriceOpenCandle(ctx)

            ctx.restore()
        }
    }

    Rectangle {
        width: 1
        anchors.top: parent.top
        anchors.bottom: timeAxisContainer.top
        anchors.right: priceCanvas.left
        color: "#3a3a3a"
    }

    // Область графика
    Flickable {
        id: chartScrollView
        anchors.left: parent.left
        anchors.right: priceCanvas.left
        anchors.top: parent.top
        anchors.bottom: timeAxisContainer.top

        flickableDirection: Flickable.HorizontalFlick
        contentWidth: Math.max(parent.width, internal.totalChartWidth)
        contentHeight: height
        boundsBehavior: Flickable.StopAtBounds
        //interactive: internal.totalChartWidth > width
        clip: true

        MouseArea {
            id: zoomArea
            anchors.fill: parent
            acceptedButtons: Qt.NoButton
            hoverEnabled: true

            onWheel: function(wheel) {
                var hasData = root.candleSeries && root.candleSeries.length > 0;
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
                    var canScroll = internal.totalChartWidth > chartScrollView.width;
                    if (canScroll) {
                        var delta = wheel.angleDelta.y / 120 * 50;
                        var newX = chartScrollView.contentX - delta;
                        var maxScrollX = Math.max(0, chartScrollView.contentWidth - chartScrollView.width);
                        chartScrollView.contentX = Math.max(0, Math.min(newX, maxScrollX));
                        wheel.accepted = true;
                    } else
                        wheel.accepted = false;
                }
            }

            onPositionChanged: function(mouse) {
                internal.mouseInside = true;
                internal.mouseGlobalX = mouse.x;
                internal.mouseGlobalY = mouse.y;
                internal.mouseLocalX = mouse.x - chartScrollView.contentX
                internal.mouseLocalY = mouse.y - chartScrollView.contentY

                // Oбновляем кросс-курсор
                internal.updateCrosshair();
            }

            onExited: {
                internal.mouseInside = false;
                internal.mouseGlobalX = -1;
                internal.mouseGlobalY = -1;
                internal.hoveredCandleIndex = -1;
                internal.timeAtCursor = "";
                crosshairCanvas.requestPaint();
                priceCanvas.requestPaint();
                timeCanvas.requestPaint();
            }
        }

        onContentXChanged: {
            if (!internal.updatingScroll)
                internal.updateScrollPosition()

            internal.updateVisibleRange()
            timeCanvas.requestPaint()
            internal.updateCrosshair()

            if (contentX <= 0)
                root.leftBoundaryReached()

        }

        onWidthChanged: {
            chartScrollView.contentWidth = Math.max(chartScrollView.parent.width, internal.totalChartWidth);

            if (internal.totalChartWidth <= width) {
                chartScrollView.contentX = 0;
                internal.scrollPosition = 0;
            } else if (internal.autoScrollEnabled && !internal.isUserInteracting)
                internal.scrollToLastCandle(false)
            else {
                var maxScrollX = Math.max(0, chartScrollView.contentWidth - chartScrollView.width);
                chartScrollView.contentX = Math.min(internal.scrollPosition * maxScrollX, maxScrollX);
            }
            internal.updateVisibleRange();
            timeCanvas.requestPaint();
            internal.updateCrosshair();
        }

        onMovementStarted: {
            if (internal.totalChartWidth > width) {
                internal.isUserInteracting = true;
                internal.autoScrollEnabled = false;
                internal.isScrolling = true;
                autoScrollRestartTimer.start();
                internal.updateCrosshair();
            }
        }

        onMovementEnded: {
            if (internal.totalChartWidth > width) {
                internal.isUserInteracting = false;
                internal.isScrolling = false;
                internal.updateScrollPosition();
                internal.updateCrosshair();
            }
        }

        Timer {
            id: autoScrollRestartTimer
            interval: root.timeAutoScroll

            onTriggered: {
                if (root.enableAutoScroll) {
                    internal.autoScrollEnabled = true
                    if (internal.totalChartWidth > chartScrollView.width)
                        internal.scrollToLastCandle(true)

                }
            }
        }

        Canvas {
            id: chartCanvas
            width: chartScrollView.contentWidth
            height: chartScrollView.contentHeight

            function drawGridChart(ctx) {
                if (!root.candleSeries)
                    return

                ctx.save()
                ctx.resetTransform()

                var priceRange = root.maxPrice - root.minPrice
                var step = internal.calculateGridStep(priceRange, 5)
                var firstPrice = Math.ceil(root.minPrice / step) * step

                ctx.strokeStyle = "#2d2d2d"
                ctx.lineWidth = 1

                for (var price = firstPrice; price <= root.maxPrice; price += step) {
                    var yGrid = Math.round(internal.getPriceY(price, height))

                    ctx.beginPath()
                    ctx.moveTo(0, yGrid)
                    ctx.lineTo(width, yGrid)
                    ctx.stroke()
                }

                ctx.restore()
            }

            function drawCandle(ctx, xPos, candle) {
                var yHigh = internal.getPriceY(candle.high, height);
                var yLow = internal.getPriceY(candle.low, height);
                var yOpen = internal.getPriceY(candle.open, height);
                var yClose = internal.getPriceY(candle.close, height);

                var color;
                if (!candle.isConfirm)
                    color = candle.close > candle.open ? "#91ffa8" : "#ff9191";
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

            function drawCandles(ctx) {
                if (!root.candleSeries || root.candleSeries.length === 0)
                    return

                // Вычисляем видимый диапазон
                var step = internal.candleWidth + internal.candleSpacing

                // Рисуем только видимые свечи
                for (var i = internal.firstVisibleIdx; i <= internal.lastVisibleIdx; i++) {
                    var candle = root.candleSeries[i]

                    if (!candle)
                        continue

                    var xPos = i * step + internal.candleSpacing / 2

                    // Рисуем вертикальные линии для каждой 5-й свечи
                    if (i % 5 === 0) {
                        ctx.strokeStyle = "#252525"
                        ctx.beginPath()
                        ctx.moveTo(xPos + internal.candleWidth / 2, 0)
                        ctx.lineTo(xPos + internal.candleWidth / 2, height)
                        ctx.stroke()
                    }

                    drawCandle(ctx, xPos, candle)
                }
            }

            function drawLineCurrentPrice(ctx) {
                var lastCandle = internal.getLastCandle()
                if (lastCandle && !lastCandle.isConfirm) {

                    var curPriceY = internal.getPriceY(lastCandle.close, height)
                    var isUp = lastCandle.close > lastCandle.open

                    ctx.save()
                    ctx.resetTransform()

                    // Горизонтальная линия текущей цены на весь canvas
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
                var ctx = getContext("2d")

                ctx.clearRect(0, 0, width, height);

                ctx.save();

                drawGridChart(ctx)

                drawCandles(ctx)

                if (root.enableDisplayOpenPrice)
                    drawLineCurrentPrice(ctx)

                ctx.restore()
            }
        }

        // Canvas для кросс-курсора (поверх всего)
        Canvas {
            id: crosshairCanvas
            anchors.fill: parent
            z: 10

            function drawCursorLines(ctx) {
                ctx.strokeStyle = "rgba(255, 255, 255, 0.3)";
                ctx.lineWidth = 1;
                ctx.setLineDash([5, 5]);

                ctx.beginPath();
                ctx.moveTo(internal.mouseGlobalX, 0);
                ctx.lineTo(internal.mouseGlobalX, height);
                ctx.stroke();

                ctx.beginPath();
                ctx.moveTo(0, internal.mouseGlobalY);
                ctx.lineTo(width, internal.mouseGlobalY);
                ctx.stroke();
            }

            function drawCursorIndicator(ctx) {
                ctx.beginPath();
                ctx.arc(internal.mouseGlobalX, internal.mouseGlobalY, 4, 0, Math.PI * 2);
                ctx.fillStyle = "rgba(255, 255, 255, 0.5)";
                ctx.fill();
                ctx.strokeStyle = "white";
                ctx.lineWidth = 1;
                ctx.stroke();
            }

            function drawCursorInfoAboutCandle(ctx) {
                if (internal.hoveredCandleIndex >= 0 && internal.hoveredCandleIndex < root.candleSeries.length) {
                    var candle = root.candleSeries[internal.hoveredCandleIndex];
                    if (candle) {
                        var dif = candle.close - candle.open
                        var difPercent = dif / candle.close * 100

                        var infoLines = [
                            "O: " + candle.open.toFixed(2),
                            "H: " + candle.high.toFixed(2),
                            "L: " + candle.low.toFixed(2),
                            "C: " + candle.close.toFixed(2),
                            "D: " + dif.toFixed(2) + " (" + difPercent.toFixed(2) + "%)"
                        ];

                        ctx.font = "10px monospace";
                        ctx.textBaseline = "top";

                        var maxTextWidth = 0;
                        for (var i = 0; i < infoLines.length; i++) {
                            var lineWidth = ctx.measureText(infoLines[i]).width;
                            if (lineWidth > maxTextWidth)
                                maxTextWidth = lineWidth;
                        }

                        var textWidth = maxTextWidth + 16;
                        var lineHeight = 14;
                        var textHeight = (lineHeight * infoLines.length) + 8;

                        var xPos = internal.mouseGlobalX + 15;
                        var yPos = internal.mouseGlobalY - 9;

                        var l_xPos = internal.mouseLocalX + 15;
                        var l_yPos = internal.mouseLocalY - 9;

                        //console.log(xPos + textWidth, width, "|", l_xPos + textWidth, chartScrollView.width)

                        if (l_xPos + textWidth > chartScrollView.width)
                            xPos = internal.mouseGlobalX - textWidth - 15

                        if (l_yPos + textHeight > chartScrollView.height)
                            yPos = height - textHeight - 5

                        if (l_yPos < 0)
                            yPos = 5

                        ctx.fillStyle = "rgba(0, 0, 0, 0.75)"
                        ctx.fillRect(xPos, yPos, textWidth, textHeight)

                        ctx.fillStyle = candle.close > candle.open ? "#00C853" : "#FF5252"

                        for (var j = 0; j < infoLines.length; j++)
                            ctx.fillText(infoLines[j], xPos + 8, yPos + 4 + (j * lineHeight));

                    }
                }
            }

            // Автоматическое обновление при изменении свойств
            Connections {
                target: internal
                function onMouseGlobalXChanged() {
                    crosshairCanvas.requestPaint();
                }
                function onMouseGlobalYChanged() {
                    crosshairCanvas.requestPaint();
                }
                function onHoveredCandleIndexChanged() {
                    crosshairCanvas.requestPaint();
                }
                function onMouseInsideChanged() {
                    crosshairCanvas.requestPaint();
                }
                function onTimeAtCursorChanged() {
                    crosshairCanvas.requestPaint();
                }
                function onPriceAtCursorChanged() {
                    crosshairCanvas.requestPaint();
                }
            }

            onPaint: {
                var ctx = getContext("2d");
                ctx.clearRect(0, 0, width, height)

                if (!internal.mouseInside || internal.mouseGlobalX < 0 || internal.mouseGlobalY < 0)
                    return;

                ctx.save()
                ctx.scale(1,1)

                // Рисуем горизонтальную и вертикальную линии
                drawCursorLines(ctx)

                ctx.setLineDash([]);

                // Рисуем кружок на пересечении
                drawCursorIndicator(ctx)

                // Рисуем информация о свече
                if (root.enableCursorInfo)
                    drawCursorInfoAboutCandle(ctx)

                ctx.restore();
            }
        }
    }

    Rectangle {
        id: timeAxisContainer
        height: internal.timeAxisHeight
        anchors.left: parent.left
        anchors.right: priceCanvas.left
        anchors.bottom: parent.bottom
        color: "#1e1e1e"
        border.width: 1
        border.color: "#3a3a3a"
        clip: true

        Canvas {
            id: timeCanvas
            width: chartScrollView.contentWidth
            height: parent.height
            x: -chartScrollView.contentX

            function drawTimeAxis(ctx) {
                if (!root.candleSeries)
                    return;

                ctx.fillStyle = "#8a8a8a";
                ctx.font = "10px sans-serif";
                ctx.textAlign = "center";
            }

            function drawDivisionsTimeAxis(ctx) {
                var step = internal.candleWidth + internal.candleSpacing;
                var offsetX = -x;
                var containerWidth = width;

                var visibleStart = Math.max(0, Math.floor(offsetX / step));
                var visibleEnd = Math.min(root.candleSeries.length - 1, Math.ceil((offsetX + containerWidth) / step));

                visibleStart = Math.max(0, visibleStart - 1);
                visibleEnd = Math.min(root.candleSeries.length - 1, visibleEnd + 1);

                var labelStep = 5
                if (internal.candleWidth < 8)
                    labelStep = 20
                else if (internal.candleWidth < 15)
                    labelStep = 10
                else if (internal.candleWidth > 40)
                    labelStep = 2
                else if (internal.candleWidth > 60)
                    labelStep = 1


                for (var i = visibleStart; i <= visibleEnd; i++) {
                    if (i % labelStep === 0) {
                        var candle = root.candleSeries[i]

                        if (!candle)
                            continue

                        var xPos = i * step + internal.candleSpacing / 2
                        //var timeStr = Qt.formatDateTime(new Date(candle.timestamp * 1000), "HH:mm")
                        var timeStr = Qt.formatDateTime(new Date(candle.end), "HH:mm")
                        ctx.fillText(timeStr, xPos + internal.candleWidth / 2, height / 2 + 3)
                    }
                }
            }

            function drawCursorTime(ctx) {
                var containerWidth = width;
                var step = internal.candleWidth + internal.candleSpacing;
                if (internal.mouseInside && internal.mouseGlobalX >= 0 && internal.mouseGlobalX <= containerWidth && internal.timeAtCursor !== "") {
                    // Проверяем, попадает ли курсор на свечу
                    var result = internal.getCandleAtX(internal.mouseGlobalX);
                    if (result && result.index >= 0 && result.index < root.candleSeries.length) {
                        // Получаем позицию свечи
                        var candleIndex = result.index;
                        var candleXPos = candleIndex * step + internal.candleSpacing / 2;
                        var candleCenterX = candleXPos + internal.candleWidth / 2;

                        // Определяем, где рисовать подсказку - над или под осью
                        var timeText = internal.timeAtCursor;
                        ctx.font = "bold 10px sans-serif";
                        var textWidth = ctx.measureText(timeText).width + 12;
                        var textHeight = 18;

                        // Позиционируем текст
                        var textX = candleCenterX - textWidth / 2;
                        var textY = height - textHeight - 2;

                        // Корректируем, чтобы не выходил за границы
                        if (textX < 0)
                            textX = 0;
                        if (textX + textWidth > width)
                            textX = width - textWidth;

                        //console.log(timeText)
                        //console.log("timeX:", textX, "timeY:", textY)

                        // Рисуем фон
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

                        // Рисуем текст с временем
                        ctx.fillStyle = "#ffffff";
                        ctx.font = "bold 10px sans-serif";
                        ctx.textAlign = "center";
                        ctx.textBaseline = "middle";
                        ctx.fillText(timeText, textX + textWidth / 2, textY + textHeight / 2);

                        // Вертикальная линия-индикатор
                        ctx.strokeStyle = "rgba(255, 255, 255, 0.15)";
                        ctx.lineWidth = 1;
                        ctx.setLineDash([3, 3]);
                        ctx.beginPath();
                        ctx.moveTo(candleCenterX, 0);
                        ctx.lineTo(candleCenterX, height);
                        ctx.stroke();
                        ctx.setLineDash([]);
                    }
                }
            }


            Connections {
                target: chartScrollView
                function onContentXChanged() {
                    timeCanvas.requestPaint();
                }
                function onWidthChanged() {
                    timeCanvas.requestPaint();
                }
            }

            Connections {
                target: root.candleSeries
                ignoreUnknownSignals: true
                function onCountChanged() {
                    Qt.callLater(function() {
                        timeCanvas.requestPaint();
                    });
                }
                function onDataChanged() {
                    timeCanvas.requestPaint();
                }
            }

            Connections {
                target: internal
                function onMouseGlobalXChanged() {
                    timeCanvas.requestPaint();
                }
                function onMouseInsideChanged() {
                    timeCanvas.requestPaint();
                }
                function onTimeAtCursorChanged() {
                    timeCanvas.requestPaint();
                }
            }

            onPaint: {
                var ctx = getContext("2d")
                ctx.clearRect(0, 0, width, height)

                ctx.save()
                ctx.scale(1,1)

                // Рисуем ось Ox
                drawTimeAxis(ctx)

                // Рисуем деления на оси Ox
                drawDivisionsTimeAxis(ctx)

                // Рисуем текущее время курсора
                if (root.enableCursorTime)
                    drawCursorTime(ctx)

                ctx.restore()
            }
        }
    }
}