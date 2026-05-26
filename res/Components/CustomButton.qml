import QtQuick
import QtQuick.Controls

import Theme 1.0

Button {
    id: root
    background: Rectangle {
        color: root.pressed ? Theme.pressColor : ( (root.hovered && root.enabled) ? Theme.hoverColor : Theme.buttonColor )
        border.color: Theme.borderColor
        border.width: Theme.borderWidth
        radius: Theme.radius
    }
    contentItem: Text {
        text: root.text
        font.family: Theme.fontFamily
        font.pixelSize: Theme.fontSizeBody
        padding: Theme.padding
        color: Theme.textColor
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}