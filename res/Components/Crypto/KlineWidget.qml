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

    RowLayout {
        anchors.fill: parent

        CustomCandleChart {
            Layout.fillHeight: true
            Layout.fillWidth: true

            candleSeries: Engine.loadedCandles

            onLeftBoundaryReached: {
                console.log("The chart reached left boundary")
                var firstCandle = candleSeries[0]
                if (firstCandle && firstCandle.start) {
                    var startTime = firstCandle.start
                    var endTime = startTime + root.numberHistoricalCandles * 60000
                    Engine.loadCandlesFromNetwork(currentTrade, "1", startTime, endTime)
                }
            }

        }
    }

}
