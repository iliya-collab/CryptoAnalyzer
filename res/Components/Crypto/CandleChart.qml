import QtQuick 2.15

Rectangle {
    id: root

    color: "#1e1e1e"
    border.width: 1
    border.color: "#3a3a3a"

    // Свойства
    property var candleSeries: null
    property real maxPrice: 100
    property real minPrice: 0
    property bool enableAutoScroll: true
    property bool enableCursorTime: true
    property bool enableCursorPrice: true
    property bool enableCursorInfo: true
    property bool enableDisplayOpenPrice: true
    property bool showVolumes: true
    property int timeAutoScroll: 3000
    property real volumeChartHeightRatio: 0.25 // 25% высоты для объемов

    signal leftBoundaryReached()

    QtObject {
        id: internal

        // Ширина свечи
        property int candleWidth: 20
        // Оступы между свечой
        property int candleSpacing: 6
        // Ширина оси цен
        property int priceAxisWidth: 60
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
        property int firstVisibleIdx: 0
        property int lastVisibleIdx: -1

        // Свойства для кросс-курсора
        property real mouseGlobalX: -1
        property real mouseGlobalY: -1
        property real mouseLocalX: -1
        property real mouseLocalY: -1
        property bool mouseInside: false
        property int hoveredCandleIndex: -1

        // Данные на осях
        property real priceAtCursor: 0
        property real volumeAtCursor: 0
        property string timeAtCursor: ""

        // Максимальный объем
        property real maxVolume: 1

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
                return { start: 0, end: -1 }

            var step = internal.candleWidth + internal.candleSpacing
            var effectiveX = Math.max(0, priceChartScrollView.contentX)
            var buffer = 2
            var firstIdx = Math.max(0, Math.floor(effectiveX / step) - buffer)
            var lastIdx = Math.min(root.candleSeries.length - 1,
                                   Math.ceil((effectiveX + priceChartScrollView.width) / step) + buffer)
            return { start: firstIdx, end: lastIdx }
        }

        // Обновляет макс и мин значения в облости видимых индексов и вызывает перерисовку всех компонентов
        function updateVisibleRange() {

            if (scrollAnimation.running)
                return;

            if (!root.candleSeries || root.candleSeries.length === 0)
                return;


            var visibleIdx = getVisibleCandleIndices();
            internal.firstVisibleIdx = visibleIdx.start;
            internal.lastVisibleIdx = visibleIdx.end;

            var currentMax = -Infinity;
            var currentMin = Infinity;
            var currentMaxVolume = 0;

            for (var i = internal.firstVisibleIdx; i <= internal.lastVisibleIdx; i++) {
                var candle = root.candleSeries[i];
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
                root.maxPrice = currentMax + padding;
                root.minPrice = currentMin - padding;
            }

            if (currentMaxVolume > internal.maxVolume)
                internal.maxVolume = currentMaxVolume * 1.2;

            requestPaintAll();
        }

        // Перематывает к последней свече с обновлением видимого диапозона
        function scrollToLastCandle(animated) {
            if (!root.candleSeries || root.candleSeries.length === 0)
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
                    //internal.updateCrosshair()
                    //internal.updateAxes()
                    internal.syncScrollViews()
                    internal.updateVisibleRange()
                    internal.updatingScroll = false
                });
            }
        }

        // Получает индекс свечи по координате x
        function getCandleAtX(x) {
            if (!root.candleSeries || root.candleSeries.length === 0)
                return null;

            var step = internal.candleWidth + internal.candleSpacing
            var index = Math.floor((x - internal.candleSpacing / 2) / step)

            if (index < 0 || index >= root.candleSeries.length)
                return null;

            var candleStartX = index * step + internal.candleSpacing / 2
            var candleEndX = candleStartX + internal.candleWidth

            if (x < candleStartX || x > candleEndX)
                return null;

            return { index: index };
        }

        function convertPriceToY(price, availableHeight) {
            var range = root.maxPrice - root.minPrice
            if (range === 0)
                return availableHeight / 2;
            return availableHeight - ((price - root.minPrice) / range) * availableHeight;
        }

        function convertYToPrice(y, availableHeight) {
            var range = root.maxPrice - root.minPrice;
            if (range === 0)
                return root.maxPrice;
            return root.maxPrice - (y / availableHeight) * range;
        }

        function convertXToTime(x) {
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

        function updateAxes() {
            priceCanvas.requestPaint();
            volumeCanvas.requestPaint();
            timeCanvas.requestPaint();
        }

        function updateCharts() {
            priceChartCanvas.requestPaint();
            volumeChartCanvas.requestPaint();
        }

        function updateCrosshair() {
            if (!internal.mouseInside || internal.mouseGlobalX < 0 || internal.mouseGlobalY < 0) {
                priceCrosshairCanvas.requestPaint()
                volumeCrosshairCanvas.requestPaint()
                return;
            }

            var result = internal.getCandleAtX(internal.mouseGlobalX);
            if (result && result.index >= 0 && result.index < root.candleSeries.length) {
                internal.hoveredCandleIndex = result.index;
                var candle = root.candleSeries[result.index];
            } else
                internal.hoveredCandleIndex = -1;

            var timestamp = internal.convertXToTime(internal.mouseGlobalX)
            internal.timeAtCursor = (timestamp > 0) ? Qt.formatDateTime(new Date(timestamp), "dd.MM.yyyy HH:mm") : ""
            internal.priceAtCursor = internal.convertYToPrice(internal.mouseGlobalY, priceChartScrollView.height)

            priceCrosshairCanvas.requestPaint()
            volumeCrosshairCanvas.requestPaint()
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
            internal.updateCharts()
            internal.updateCrosshair()
            internal.updateAxes()
        }

        function update() {
            if (!root.candleSeries || root.candleSeries.length === 0) {
                requestPaintAll();
                return;
            }

            // Устанавливаем ширину контента
            priceChartScrollView.contentWidth = Math.max(root.width, internal.totalChartWidth);
            // Синхронизируем ширину для графика объемов
            //volumeChartScrollView.contentWidth = priceChartScrollView.contentWidth;
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

        function getLastCandle() {
            var lastIdx = root.candleSeries.length - 1
            if (lastIdx < 0)
                return null;
            return root.candleSeries[lastIdx]
        }

        // Синхронизация скролла между графиками
        function syncScrollViews() {
            var safeX = Math.max(0, priceChartScrollView.contentX);
            if (volumeChartScrollView.contentX !== safeX)
                volumeChartScrollView.contentX = safeX;
            volumeChartScrollView.contentWidth = priceChartScrollView.contentWidth;
        }
    }


    onCandleSeriesChanged: {
        internal.scrollPosition = 0;
        Qt.callLater(function() {
            internal.update();
        });
    }

    Component.onCompleted: {
        if (root.candleSeries && root.candleSeries.length > 0) {
            Qt.callLater(function() {
                internal.update();
                root.update();
            });
        }
    }

    function zoomIn() {
        var newWidth = Math.min(internal.candleWidth + internal.zoomStep, internal.maxCandleWidth);

        if (newWidth !== internal.candleWidth) {
            internal.saveScrollPosition()
            internal.updatingScroll = true

            internal.candleWidth = newWidth
            priceChartScrollView.contentWidth = Math.max(root.width, internal.totalChartWidth)

            if (internal.totalChartWidth <= priceChartScrollView.width) {
                priceChartScrollView.contentX = 0
                internal.scrollPosition = 0
            } else
                internal.restoreScrollPosition(false)

            internal.updatingScroll = false

            internal.updateVisibleRange()
        }
    }

    function zoomOut() {
        var newWidth = Math.max(internal.candleWidth - internal.zoomStep, internal.minCandleWidth);

        if (newWidth !== internal.candleWidth) {
            internal.saveScrollPosition();
            internal.updatingScroll = true;

            internal.candleWidth = newWidth;
            priceChartScrollView.contentWidth = Math.max(root.width, internal.totalChartWidth);

            if (internal.totalChartWidth <= priceChartScrollView.width) {
                priceChartScrollView.contentX = 0;
                internal.scrollPosition = 0;
            } else {
                internal.restoreScrollPosition(false);
            }

            internal.updatingScroll = false;

            internal.updateVisibleRange();
        }
    }

    function resetZoom() {
        internal.saveScrollPosition();
        internal.updatingScroll = true;

        internal.candleWidth = 20;
        priceChartScrollView.contentWidth = Math.max(root.width, internal.totalChartWidth);

        if (internal.totalChartWidth <= priceChartScrollView.width) {
            priceChartScrollView.contentX = 0;
            internal.scrollPosition = 0;
        } else {
            internal.restoreScrollPosition(true);
        }

        internal.updatingScroll = false;

        internal.updateVisibleRange();
    }

    NumberAnimation {
        id: scrollAnimation
        target: priceChartScrollView
        property: "contentX"
        duration: 200
        easing.type: Easing.OutCubic
        onStopped: {
            internal.syncScrollViews();
            internal.updateVisibleRange();
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
                    internal.scrollToLastCandle(true);
            }
        }
    } // autoScrollRestartTimer

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
            internal.mouseInside = true
            internal.mouseGlobalX = mouse.x + priceChartScrollView.contentX
            internal.mouseGlobalY = mouse.y + priceChartScrollView.contentY
            internal.mouseLocalX = mouse.x
            internal.mouseLocalY = mouse.y

            //console.info("Local:", internal.mouseLocalX, internal.mouseLocalY, "Global:", internal.mouseGlobalX, internal.mouseGlobalY)

            internal.updateCrosshair()
            internal.updateAxes()
        }

        onExited: {
            internal.mouseInside = false
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

    // Область ценового графика (верхняя часть)
    Rectangle {
        id: priceArea
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        height: root.showVolumes ? (parent.height - internal.timeAxisHeight) * (1 - root.volumeChartHeightRatio)
                                 : parent.height - internal.timeAxisHeight
        color: "transparent"

        // Обработчики изменения размеров для принудительной перерисовки
        onWidthChanged: {
            priceCanvas.requestPaint();
            priceChartCanvas.requestPaint();
        }
        onHeightChanged: {
            priceCanvas.requestPaint();
            priceChartCanvas.requestPaint();
        }

        // Информация о свече
        Rectangle {
            id: candleInfoBox
            anchors.left: parent.left
            anchors.top: parent.top
            width: infoLayout.width + 16
            height: infoLayout.height + 16
            color: "transparent"
            enabled: root.enableCursorInfo
            z: 20

            Row {
                id: infoLayout
                anchors.centerIn: parent
                spacing: 10
                padding: 2

                Text {
                    text: {
                        var candle = root.candleSeries ? root.candleSeries[internal.hoveredCandleIndex] : null;
                        if (!candle)
                            return "O: —";
                        return "O: " + candle.open.toFixed(2);
                    }
                    color: "#8a8a8a"
                    font.pixelSize: 11
                    font.family: "monospace"
                }
                Text {
                    text: {
                        var candle = root.candleSeries ? root.candleSeries[internal.hoveredCandleIndex] : null;
                        if (!candle)
                            return "H: —";
                        return "H: " + candle.high.toFixed(2);
                    }
                    color: "#8a8a8a"
                    font.pixelSize: 11
                    font.family: "monospace"
                }
                Text {
                    text: {
                        var candle = root.candleSeries ? root.candleSeries[internal.hoveredCandleIndex] : null;
                        if (!candle)
                            return "L: —";
                        return "L: " + candle.low.toFixed(2);
                    }
                    color: "#8a8a8a"
                    font.pixelSize: 11
                    font.family: "monospace"
                }
                Text {
                    text: {
                        var candle = root.candleSeries ? root.candleSeries[internal.hoveredCandleIndex] : null;
                        if (!candle)
                            return "C: —";
                        return "C: " + candle.close.toFixed(2);
                    }
                    color: "#8a8a8a"
                    font.pixelSize: 11
                    font.family: "monospace"
                }
                Text {
                    text: {
                        var candle = root.candleSeries ? root.candleSeries[internal.hoveredCandleIndex] : null;
                        if (!candle)
                            return "Δ: —";
                        var dif = candle.close - candle.open;
                        var percent = (dif / candle.open) * 100;
                        return "Δ: " + dif.toFixed(2) + " (" + percent.toFixed(2) + "%)";
                    }
                    color: {
                        var candle = root.candleSeries ? root.candleSeries[internal.hoveredCandleIndex] : null;
                        if (!candle)
                            return "#8a8a8a";
                        return (candle.close > candle.open) ? "#66BB6A" : "#EF5350";
                    }
                    font.pixelSize: 11
                    font.family: "monospace"
                }
                Text {
                    text: {
                        var candle = root.candleSeries ? root.candleSeries[internal.hoveredCandleIndex] : null;
                        if (!candle)
                            return "V: —";
                        return "V: " + (candle.volume >= 1000 ? (candle.volume/1000).toFixed(2) + "K" : candle.volume.toFixed(2));
                    }
                    color: "#8a8a8a"
                    font.pixelSize: 11
                    font.family: "monospace"
                }
                Text {
                    text: {
                        var candle = root.candleSeries ? root.candleSeries[internal.hoveredCandleIndex] : null;
                        if (!candle)
                            return "T: —";
                        return "T: " + (candle.turnover ? candle.turnover.toFixed(0) : "—");
                    }
                    color: "#8a8a8a"
                    font.pixelSize: 11
                    font.family: "monospace"
                }
            }
        }

        // Правая ось цен
        Canvas {
            id: priceCanvas
            width: internal.priceAxisWidth
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
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
                    var yPos = Math.round(internal.convertPriceToY(price, height));
                    var decimalPlaces = step < 1 ? 2 : (step % 1 === 0 ? 0 : 1);
                    ctx.fillText(price.toFixed(decimalPlaces), 8, yPos);
                }
            }

            function drawCursorPrice(ctx) {
                if (internal.mouseInside && internal.mouseGlobalX >= 0 && internal.mouseGlobalY <= height) {
                    var cursorY = internal.mouseGlobalY;

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
                if (width === 0 || height === 0)
                    return;
                var ctx = getContext("2d")
                ctx.clearRect(0, 0, width, height);
                ctx.save()
                ctx.scale(1, 1)

                drawPriceAxis(ctx)
                drawDivisionsPriceAxis(ctx)
                if (root.enableCursorPrice)
                    drawCursorPrice(ctx)
                if (root.enableDisplayOpenPrice)
                    drawPriceOpenCandle(ctx)

                ctx.restore()
            }
        }

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
                    internal.updatingScroll = true;
                    var clampedX = Math.max(0, contentX);
                    if (volumeChartScrollView.contentX !== clampedX)
                        volumeChartScrollView.contentX = clampedX;
                    internal.updateScrollPosition();
                    internal.updateVisibleRange();
                    internal.updatingScroll = false;
                }
                if (contentX <= 0)
                    root.leftBoundaryReached()
            }

            onWidthChanged: {
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
                width: priceChartScrollView.contentWidth
                height: priceChartScrollView.contentHeight

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

                function drawCandles(ctx) {
                    if (!root.candleSeries || root.candleSeries.length === 0)
                        return

                    var startIdx = internal.firstVisibleIdx;
                    var endIdx = internal.lastVisibleIdx;
                    if (startIdx < 0 || endIdx < 0) {
                        startIdx = 0;
                        endIdx = root.candleSeries.length - 1;
                    }

                    var step = internal.candleWidth + internal.candleSpacing
                    for (var i = startIdx; i <= endIdx; i++) {
                        var candle = root.candleSeries[i]
                        if (!candle) continue
                        var xPos = i * step + internal.candleSpacing / 2

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

            // Кросс-курсор для ценового графика
            Canvas {
                id: priceCrosshairCanvas
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

                Connections {
                    target: internal
                    function onMouseGlobalXChanged() {
                        priceCrosshairCanvas.requestPaint();
                    }
                    function onMouseGlobalYChanged() {
                        priceCrosshairCanvas.requestPaint();
                    }
                    function onHoveredCandleIndexChanged() {
                        priceCrosshairCanvas.requestPaint();
                    }
                    function onMouseInsideChanged() {
                        priceCrosshairCanvas.requestPaint();
                    }
                    function onTimeAtCursorChanged() {
                        priceCrosshairCanvas.requestPaint();
                    }
                    function onPriceAtCursorChanged() {
                        priceCrosshairCanvas.requestPaint();
                    }
                }

                onPaint: {
                    var ctx = getContext("2d");
                    ctx.clearRect(0, 0, width, height)

                    if (!internal.mouseInside || internal.mouseGlobalX < 0 || internal.mouseGlobalY < 0)
                        return;

                    ctx.save()
                    ctx.scale(1,1)

                    drawCursorLines(ctx)
                    ctx.setLineDash([]);
                    drawCursorIndicator(ctx)

                    ctx.restore();
                }
            }
        }

    }

    // Разделитель между ценовым графиком и объемами
    Rectangle {
        id: priceVolumeSeparator
        height: root.showVolumes ? 1 : 0
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: priceArea.bottom
        color: "#3a3a3a"
        z: 5
        visible: root.showVolumes
    }

    // Область графика объемов (нижняя часть)
    Rectangle {
        id: volumeArea
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: priceVolumeSeparator.bottom
        height: root.showVolumes ? (parent.height - internal.timeAxisHeight) * root.volumeChartHeightRatio : 0
        visible: root.showVolumes
        color: "transparent"

        // Обработчики изменения размеров для принудительной перерисовки
        onWidthChanged: {
            volumeCanvas.requestPaint();
            volumeChartCanvas.requestPaint();
        }
        onHeightChanged: {
            volumeCanvas.requestPaint();
            volumeChartCanvas.requestPaint();
        }

        // Правая ось для объемов
        Canvas {
            id: volumeCanvas
            width: internal.priceAxisWidth
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            visible: root.showVolumes
            z: 2

            function drawVolumeAxis(ctx) {
                ctx.fillStyle = "#8a8a8a";
                ctx.font = "10px sans-serif";
                ctx.textBaseline = "middle";
                //ctx.textAlign = "right";
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
                    volumeCanvas.requestPaint();
                }
                function onMouseInsideChanged() {
                    volumeCanvas.requestPaint();
                }
            }

            onPaint: {
                if (width === 0 || height === 0)
                    return;
                var ctx = getContext("2d")
                ctx.clearRect(0, 0, width, height);
                ctx.save()

                drawVolumeAxis(ctx)
                drawDivisionsVolumeAxis(ctx)

                ctx.restore()
            }
        }

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
            visible: root.showVolumes
            clip: true
            interactive: true
            boundsBehavior: Flickable.StopAtBounds

            contentWidth: priceChartScrollView.contentWidth
            contentHeight: height

            onContentXChanged: {
                if (!internal.updatingScroll) {
                    internal.updatingScroll = true;
                    var clampedX = Math.max(0, contentX);
                    if (priceChartScrollView.contentX !== clampedX)
                        priceChartScrollView.contentX = clampedX;
                    internal.updateScrollPosition();
                    internal.updatingScroll = false;
                }
            }

            Canvas {
                id: volumeChartCanvas
                width: volumeChartScrollView.contentWidth
                height: volumeChartScrollView.contentHeight

                function drawVolumes(ctx) {
                    if (!root.candleSeries || root.candleSeries.length === 0 || internal.maxVolume === 0)
                        return;

                    var startIdx = internal.firstVisibleIdx;
                    var endIdx = internal.lastVisibleIdx;
                    if (startIdx < 0 || endIdx < 0) {
                        startIdx = 0;
                        endIdx = root.candleSeries.length - 1;
                    }

                    var step = internal.candleWidth + internal.candleSpacing;
                    var totalHeight = height;

                    for (var i = startIdx; i <= endIdx; i++) {
                        var candle = root.candleSeries[i];

                        if (!candle || candle.volume === undefined || candle.volume === 0)
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

                        if (i % 5 === 0) {
                            ctx.strokeStyle = "#252525"
                            ctx.beginPath()
                            ctx.moveTo(xPos + internal.candleWidth / 2, 0)
                            ctx.lineTo(xPos + internal.candleWidth / 2, totalHeight)
                            ctx.stroke()
                        }
                    }
                }

                function drawVolumeHorizontalLines(ctx) {
                    if (internal.maxVolume === 0)
                        return;

                    ctx.save()
                    ctx.resetTransform()

                    var targetLines = 3;
                    var step = internal.calculateGridStep(internal.maxVolume, targetLines);
                    var firstValue = Math.ceil(0 / step) * step;

                    ctx.strokeStyle = "#2d2d2d";
                    ctx.lineWidth = 1;

                    for (var vol = firstValue; vol <= internal.maxVolume; vol += step) {
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
                    var ctx = getContext("2d");
                    ctx.clearRect(0, 0, width, height);
                    ctx.save();

                    drawVolumeHorizontalLines(ctx);
                    drawVolumes(ctx);

                    ctx.restore();
                }
            }

            Canvas {
                id: volumeCrosshairCanvas
                anchors.fill: parent
                z: 10

                function drawVolumeCursor(ctx) {
                    if (!internal.mouseInside || internal.mouseGlobalX < 0 || internal.mouseGlobalY < 0)
                        return;

                    // Вертикальная линия
                    ctx.save();
                    ctx.strokeStyle = "rgba(255, 255, 255, 0.3)";
                    ctx.lineWidth = 1;
                    ctx.setLineDash([5, 5]);
                    ctx.beginPath();
                    ctx.moveTo(internal.mouseGlobalX, 0);
                    ctx.lineTo(internal.mouseGlobalX, height);
                    ctx.stroke();
                    ctx.restore();

                    // (Опционально) кружок на уровне объёма текущей свечи
                    var result = internal.getCandleAtX(internal.mouseGlobalX);
                    if (result && result.index >= 0 && result.index < root.candleSeries.length) {
                        var candle = root.candleSeries[result.index];
                        if (candle && candle.volume > 0) {
                            var volHeight = (candle.volume / internal.maxVolume) * height;
                            var yPos = height - volHeight;
                            ctx.beginPath();
                            ctx.arc(internal.mouseGlobalX, yPos, 4, 0, Math.PI * 2);
                            ctx.fillStyle = "rgba(255,255,255,0.6)";
                            ctx.fill();
                            ctx.strokeStyle = "white";
                            ctx.lineWidth = 1;
                            ctx.stroke();
                        }
                    }
                }

                Connections {
                    target: internal
                    function onMouseGlobalXChanged() { volumeCrosshairCanvas.requestPaint(); }
                    function onMouseInsideChanged() { volumeCrosshairCanvas.requestPaint(); }
                }

                onPaint: {
                    var ctx = getContext("2d");
                    ctx.clearRect(0, 0, width, height);
                    drawVolumeCursor(ctx);
                }
            }
        }
    }

    // Ось времени (всегда внизу)
    Rectangle {
        id: timeAxisContainer
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
            width: priceChartScrollView.contentWidth
            height: parent.height
            x: -priceChartScrollView.contentX

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
                        var timeStr = Qt.formatDateTime(new Date(candle.end), "HH:mm")
                        ctx.fillText(timeStr, xPos + internal.candleWidth / 2, height / 2 + 3)
                    }
                }
            }

            function drawCursorTime(ctx) {
                var containerWidth = width;
                var step = internal.candleWidth + internal.candleSpacing;
                if (internal.mouseInside && internal.mouseGlobalX >= 0 && internal.mouseGlobalX <= containerWidth && internal.timeAtCursor !== "") {
                    var result = internal.getCandleAtX(internal.mouseGlobalX);
                    if (result && result.index >= 0 && result.index < root.candleSeries.length) {
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
                target: priceChartScrollView
                function onContentXChanged() {
                    timeCanvas.requestPaint();
                }
                function onWidthChanged() {
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

                drawTimeAxis(ctx)
                drawDivisionsTimeAxis(ctx)
                if (root.enableCursorTime)
                    drawCursorTime(ctx)

                ctx.restore()
            }
        }
    }
}