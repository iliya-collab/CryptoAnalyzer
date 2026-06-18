import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Theme 1.0
import Components.Custom 1.0

ListView {
    id: control

    property color priceColor: Theme.textColor

    clip: true

    headerPositioning: ListView.OverlayHeader

    delegate: Rectangle {
        width: control.width
        height: 25
        color: index % 2 === 0 ? "#1a1a1a" : "#252525"
        radius: Theme.radius
        border.color: Theme.borderColor
        border.width: Theme.borderWidth

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: Theme.margins
            anchors.rightMargin: Theme.margins

            CustomText {
                Layout.fillWidth: true
                text: price.toFixed(2)
                color: priceColor
            }

            CustomText {
                Layout.fillWidth: true
                text: amount.toFixed(4)
                horizontalAlignment: Text.AlignRight
            }
        }
    }
}
