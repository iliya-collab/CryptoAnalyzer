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

        KlineChart {
            id: chart
            Layout.fillHeight: true
            Layout.fillWidth: true

            onLeftBoundaryReached: {
                var firstCandle = AppCore.marketState.klineSeries.first()
                if (firstCandle && firstCandle.time)
                {
                    var endTime = firstCandle.time - 60000
                    var startTime = endTime - (root.numberHistoricalCandles - 1) * 60000
                    AppCore.marketService.loadKlines(currentTrade, "1", startTime, endTime)
                }
            }
        }
    }

}
