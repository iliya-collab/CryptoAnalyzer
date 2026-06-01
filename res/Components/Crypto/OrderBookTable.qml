import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Theme 1.0
import Components.Custom 1.0
import EngineComponents 1.0

Rectangle {
    id: root

    color: Theme.windowColor

    function update(bids, asks) {
        //var savedScrollPos = table.contentY
        orderBookModel.updateFromQml(bids, asks)
        /*Qt.callLater(function() {
            table.contentY = savedScrollPos
        })*/
    }

    OrderBookModel { id: orderBookModel }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.margins
        spacing: Theme.spacing

        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 30

            CustomText {
                Layout.fillWidth: true
                text: "Price";
                font.bold: true
                horizontalAlignment: Text.AlignLeft
            }

            CustomText {
                Layout.fillWidth: true
                text: "Amount";
                font.bold: true;
                horizontalAlignment: Text.AlignRight
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true

            border.width: Theme.borderWidth
            border.color: Theme.borderColor
            radius: Theme.radius
            color: Theme.windowColor

            ListView {
                id: table

                anchors.fill: parent
                anchors.margins: Theme.margins
                model: orderBookModel
                clip: true

                contentWidth: parent.width

                delegate: Rectangle {
                    width: table.width
                    height: 25
                    color: type === "bid" ? "#0a1f10" : "#240e0e"
                    border.width: Theme.borderWidth
                    border.color: Theme.borderColor

                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: Theme.margins
                        anchors.rightMargin: Theme.margins

                        // Цена (зеленая для bid, красная для ask)
                        CustomText {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignHCenter
                            text: price.toFixed(2)
                            color: type === "bid" ? "#00ff66" : "#ff4444"
                            horizontalAlignment: Text.AlignLeft
                        }

                        // Объём/Количество
                        CustomText {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignHCenter
                            text: amount.toFixed(4)
                            horizontalAlignment: Text.AlignRight
                        }
                    }
                }
            } // table
        } // Rectangle
    } // ColumnLayout
} // root
