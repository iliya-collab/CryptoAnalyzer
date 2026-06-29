import QtQuick 2.15
import QtQuick.Layouts 2.15
import Components.Custom 1.0
import Engine 1.0

Item {
    id: root

    property bool isNewCandle: true
    property real difTime: 0
    property int numberHistoricalCandles: 20
    property string currentTrade: ""

    // ПРЕРЕДЕЛАТЬ ПОД ПРИВЯЗКУ МОДЕЛИ
    //  * РЕАЛИЗОВАТЬ ОБНОВЛЕНИЕ ПОСЛЕДНЕЙ СВЕЧИ В AppCore::saveCandle
    //  * СДЕЛАТЬ ОБНОВЛЕНИЕ СПИСКА Engine.loadedCandles ВНУТИРИ AppCore С СООБЩЕНИЕМ ОБ ИЗМЕНЕНИИ НАРУЖУ
    //  * УБРАТЬ AppCore::saveCandle В PRIVATE

    function updateKline(newKline) {
        /*if (isNewCandle) {
            //console.log(Qt.formatTime(Date(Engine.startTime + difTime), "hh:mm:ss"))
            klineView.addCandle(Engine.startTime + difTime, newKline.open, newKline.close, newKline.high, newKline.low, true)
            isNewCandle = false
        }

        klineView.updateLastOpenCandle(newKline.close, newKline.high, newKline.low)

        if (newKline.confirm) {
            //console.log(Qt.formatTime(Date(newKline.timestamp), "hh:mm:ss"))
            difTime = difTime + 60000
            klineView.closeLastOpenCandle()
            Engine.saveCandle(newKline)
            isNewCandle = true
        }*/
    }

    function bindKline() {
        Engine.klineUpdated.connect(root.updateKline)
    }

    function unbindKline() {
        Engine.klineUpdated.disconnect(root.updateKline)
    }

    RowLayout {
        anchors.fill: parent

        CustomCandleChart {
            id: klineView
            Layout.fillHeight: true
            Layout.fillWidth: true

            candleSeries: Engine.loadedCandles

            onLeftBoundaryReached: {
                console.log("The chart reached left boundary")
                var startTime = candleSeries.get(0).timestamp
                var endTime = startTime + root.numberHistoricalCandles * 60000
                Engine.loadCandlesFromNetwork(currentTrade, "1", startTime, endTime)
            }

        }
    }

    Component.onCompleted: {
        /*var historicalData = [
            { timestamp: new Date(2024, 0, 1, 10, 0).getTime(), open: 100, close: 102, high: 103, low: 99 },
            { timestamp: new Date(2024, 0, 1, 10, 5).getTime(), open: 102, close: 101, high: 104, low: 100 },
            { timestamp: new Date(2024, 0, 1, 10, 10).getTime(), open: 101, close: 105, high: 106, low: 100 }
        ]

        klineView.addHistoricalCandles(historicalData)*/
    }

}
