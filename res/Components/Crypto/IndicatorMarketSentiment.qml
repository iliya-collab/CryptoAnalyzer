import QtQuick 2.15
import QtQuick.Layouts
import QtQuick.Controls
import Theme 1.0

Item {
    id: root

    property real percentAsk: 0
    property real percentBid: 0

    RowLayout {
        anchors.fill: parent

        // Индикатор Bid
        Rectangle {
            width: indicatorBid.contentHeight + Theme.margins
            height: indicatorBid.contentHeight + Theme.margins
            Layout.alignment: Qt.AlignVCenter
            color: "transparent"
            border.color: "#00ff66"
            border.width: Theme.borderWidth
            radius: 5
            Text {
                id: indicatorBid
                anchors.centerIn: parent
                text: "B"
                font.pixelSize: Theme.fontSizeBody
                font.family: Theme.fontFamily
                color: "#00ff66"
            } // Text
        }

        // Центральня полоса
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Row {
                anchors.fill: parent
                spacing: 0

                Rectangle {
                    id: bidSide
                    height: parent.height
                    width: parent.width * root.percentBid / 100
                    color: "#00ff66"
                    Text {
                        anchors.left: parent.left
                        anchors.leftMargin: Theme.margins / 2
                        anchors.verticalCenter: parent.verticalCenter
                        text: root.percentBid + "%"
                        font.pixelSize: Theme.fontSizeBody
                        font.family: Theme.fontFamily
                    }
                }

                Rectangle {
                    id: askSide
                    height: parent.height
                    width: parent.width * root.percentAsk / 100
                    color: "#ff4444"
                    Text {
                        anchors.right: parent.right
                        anchors.rightMargin: Theme.margins / 2
                        anchors.verticalCenter: parent.verticalCenter
                        text: root.percentAsk + "%"
                        font.pixelSize: Theme.fontSizeBody
                        font.family: Theme.fontFamily
                    }
                }
            } // Row
        } // Item

        // Индикатор Ask
        Rectangle {
            width: indicatorAsk.contentHeight + Theme.margins
            height: indicatorAsk.contentHeight + Theme.margins
            Layout.alignment: Qt.AlignVCenter
            color: "transparent"
            border.color: "#ff4444"
            border.width: Theme.borderWidth
            radius: 5
            Text {
                id: indicatorAsk
                anchors.centerIn: parent
                text: "A"
                font.pixelSize: Theme.fontSizeBody
                font.family: Theme.fontFamily
                color: "#ff4444"
            } // Text
        }

    }

}
