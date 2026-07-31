import QtQuick 2.15
import QtQuick.Layouts 2.15
import Components.Crypto 1.0
import Components.Custom 1.0
import Application.Core 1.0

Item {
    id: root

    property bool isNewCandle: true
    property real difTime: 0
    property int numberHistoricalCandles: 20
    property string currentTrade: ""
    property alias enableVolumeChart: chart.enableShowVolumes

    ColumnLayout {
        anchors.fill: parent

        // Панель зума
        Rectangle {
           color: "#2a2a2a"
           Layout.preferredHeight: 30
           Layout.preferredWidth: 300

            RowLayout {
                anchors.fill: parent
                anchors.margins: 4
                spacing: 4

                CustomLabel { text: "Zoom:" }

                CustomButton {
                    text: "-"
                    Layout.preferredWidth: 50
                    Layout.fillHeight: true
                    onClicked: chart.zoomOut()
                }

                CustomButton {
                    text: "+"
                    Layout.preferredWidth: 50
                    Layout.fillHeight: true
                    onClicked: chart.zoomIn()
                }

                CustomButton {
                    text: "⟲"
                    Layout.preferredWidth: 50
                    Layout.fillHeight: true
                    onClicked: chart.resetZoom()
                }
            }
        }

        CandleChart {
            id: chart
            Layout.fillHeight: true
            Layout.fillWidth: true

            candleSeries: AppCore.candleSeries

            onLeftBoundaryReached: {
                //console.info("The chart reached left boundary")
                var firstCandle = candleSeries[0]
                if (firstCandle && firstCandle.start) {
                    var endTime = firstCandle.start
                    var startTime = endTime - root.numberHistoricalCandles * 60000
                    //console.info(firstCandle.start, startTime, endTime)
                    AppCore.loadCandlesFromNetwork(currentTrade, "1", startTime, endTime)
                }
            }

        }
    }

}
