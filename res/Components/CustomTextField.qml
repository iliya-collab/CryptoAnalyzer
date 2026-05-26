import QtQuick
import QtQuick.Controls

import Theme 1.0

TextField {
    id: root
    background: Rectangle {
        color: Theme.textFieldColor
        border.color: root.hovered ? Theme.hoverTextFieldColor : Theme.borderColor
        border.width: Theme.borderWidth
        radius: Theme.radius
    }
    font.family: Theme.fontFamily
    font.pixelSize: Theme.fontSizeBody
    padding: Theme.padding
    color: Theme.textColor
    verticalAlignment: Text.AlignVCenter
}
