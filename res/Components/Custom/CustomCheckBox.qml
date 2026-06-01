import QtQuick
import QtQuick.Controls

import Theme 1.0

CheckBox {
    id: root
    indicator: Rectangle {
        implicitWidth: Theme.widthBox
        implicitHeight: Theme.widthBox
        x: root.leftPadding
        y: (root.height - height) / 2
        radius: Theme.radius
        border.color: root.checked ? Theme.indicatorColor : Theme.borderColor
        border.width: Theme.borderWidth
        color: "transparent"

        Text {
            anchors.centerIn: parent
            text: "✓"
            font.pixelSize: Theme.widthBox * 0.8
            color: Theme.indicatorColor
            visible: root.checked
        }
    }
    contentItem: Text {
        text: root.text
        font.pixelSize: Theme.fontSizeBody
        font.family: Theme.fontFamily
        color: Theme.textColor
        verticalAlignment: Text.AlignVCenter
        leftPadding: root.indicator.width + root.spacing
    }
}