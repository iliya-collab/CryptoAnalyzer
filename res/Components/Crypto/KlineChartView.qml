import QtQuick 2.15
import QtCharts 2.15
import QtQuick.Controls 2.15 // 1. Добавляем импорт для ScrollBar

Item {
    id: root

    // Настройки отображения
    property real candleTimeframeMs: 60 * 1000
    property real visibleRangeMs: candleTimeframeMs * 40

    // Храним глобальные границы всей доступной истории данных
    property real firstTimestamp: 0
    property real lastTimestamp: 0

    // Вспомогательный флаг, чтобы избежать зацикливания при обновлении
    property bool _updatingProperties: false

    Column {
        anchors.fill: parent
        spacing: 0

        ChartView {
            id: chartView
            width: parent.width
            height: parent.height - scrollBar.height // Оставляем место для скроллбара
            antialiasing: true
            theme: ChartView.ChartThemeDark

            DateTimeAxis {
                id: axisX
                format: "hh:mm:ss"
                tickCount: 5

                // Следим за изменениями видимой области и двигаем ползунок скроллбара
                onMinChanged: root.updateScrollBarPosition()
                onMaxChanged: root.updateScrollBarPosition()
            }

            ValueAxis {
                id: axisY
                min: 0
                max: 100
                labelFormat: "%.2f"
            }

            CandlestickSeries {
                id: candleSeries
                axisX: axisX
                axisY: axisY
                increasingColor: "#00ff00"
                decreasingColor: "#ff0000"
                bodyOutlineVisible: true
            }

            // Оставляем MouseArea для возможности скроллить график "рукой"
            MouseArea {
                anchors.fill: parent
                property real lastX: 0
                onPressed: (mouse) => lastX = mouse.x
                onPositionChanged: (mouse) => {
                    if (pressed) {
                        var dx = mouse.x - lastX
                        lastX = mouse.x
                        var msPerPixel = (axisX.max.getTime() - axisX.min.getTime()) / chartView.width
                        var timeShift = dx * msPerPixel

                        // Ограничиваем скролл, чтобы не уходить за пределы истории
                        var newMin = axisX.min.getTime() - timeShift
                        var newMax = axisX.max.getTime() - timeShift
                        if (newMin >= root.firstTimestamp && newMax <= root.lastTimestamp) {
                            axisX.min = new Date(newMin)
                            axisX.max = new Date(newMax)
                        }
                    }
                }
            }
        }

        // 2. Визуальная полоса прокрутки
        ScrollBar {
            id: scrollBar
            width: parent.width
            orientation: Qt.Horizontal
            visible: candleSeries.count > 40 // Показываем, только если свечей больше, чем влазит на экран

            // Когда пользователь тащит ползунок скроллбара мышкой
            onPositionChanged: {
                if (pressed && !root._updatingProperties) {
                    var totalRange = root.lastTimestamp - root.firstTimestamp
                    var currentVisibleWidth = axisX.max.getTime() - axisX.min.getTime()

                    var newMinTime = root.firstTimestamp + position * totalRange
                    var newMaxTime = newMinTime + currentVisibleWidth

                    axisX.min = new Date(newMinTime)
                    axisX.max = new Date(newMaxTime)
                }
            }
        }
    }

    Component {
        id: candleComponent
        CandlestickSet {}
    }

    // Функция обновления размера и позиции ползунка
    function updateScrollBarPosition() {
        if (_updatingProperties || firstTimestamp === lastTimestamp) return

        _updatingProperties = true
        var totalRange = lastTimestamp - firstTimestamp
        var visibleWidth = axisX.max.getTime() - axisX.min.getTime()

        // Размер бегунка — это отношение видимой области ко всей истории
        scrollBar.size = visibleWidth / totalRange
        // Позиция бегунка — это смещение от самого первого таймстемпа
        scrollBar.position = (axisX.min.getTime() - firstTimestamp) / totalRange
        _updatingProperties = false
    }

    function addCandle(timestamp, open, close, high, low) {
        var newCandle = candleComponent.createObject(candleSeries, {
            "timestamp": timestamp,
            "open": open,
            "close": close,
            "high": high,
            "low": low
        })

        if (newCandle !== null) {
            candleSeries.append(newCandle)

            // Динамически расширяем границы доступной истории данных
            if (candleSeries.count === 1) {
                root.firstTimestamp = timestamp - root.candleTimeframeMs
                root.lastTimestamp = timestamp + root.visibleRangeMs
            } else {
                if (timestamp > root.lastTimestamp) root.lastTimestamp = timestamp
            }

            // Автомасштаб оси Y
            if (candleSeries.count === 1) {
                axisY.min = low - 50
                axisY.max = high + 50
            } else {
                if (axisY.min > low) axisY.min = low - 50
                if (axisY.max < high) axisY.max = high + 50
            }

            var candleDate = new Date(timestamp)

            // Логика инициализации и автопрокрутки
            if (candleSeries.count === 1) {
                axisX.min = new Date(root.firstTimestamp)
                axisX.max = new Date(root.firstTimestamp + root.visibleRangeMs)
            }
            // Если график находится в самом конце истории, автоматически двигаем экран за новой свечой
            else if (candleDate > axisX.max && (root.lastTimestamp - axisX.max.getTime() <= root.candleTimeframeMs)) {
                var currentWidth = axisX.max.getTime() - axisX.min.getTime()
                axisX.max = candleDate
                axisX.min = new Date(candleDate.getTime() - currentWidth)
            }

            // Принудительно пересчитываем размер скроллбара для новой свечи
            root.updateScrollBarPosition()
        }
    }
}
