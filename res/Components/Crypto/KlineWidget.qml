import QtQuick 2.15
import QtQuick.Layouts 2.15
import Components.Crypto 1.0
import Engine 1.0

Item {
    id: root

    property bool isNewCandle: true
    property real difTime: 0
    property int numberHistoricalCandles: 20
    property string currentTrade: ""

    RowLayout {
        anchors.fill: parent

        CandleChart {
            Layout.fillHeight: true
            Layout.fillWidth: true

            candleSeries: Engine.candleSeries

            onLeftBoundaryReached: {
                //console.info("The chart reached left boundary")
                var firstCandle = candleSeries[0]
                if (firstCandle && firstCandle.start) {
                    var endTime = firstCandle.start
                    var startTime = endTime - root.numberHistoricalCandles * 60000
                    //console.info(firstCandle.start, startTime, endTime)
                    Engine.loadCandlesFromNetwork(currentTrade, "1", startTime, endTime)
                }
            }

        }
    }

}
