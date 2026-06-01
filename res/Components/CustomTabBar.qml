import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Theme 1.0

TabBar {
    id: root

    property var modelTabs: []

    background: Rectangle {
        border.color: Theme.borderColor
        border.width: Theme.borderWidth
        color: Theme.toolBarColor
        radius: Theme.radius
    }

    Repeater {
        model: root.modelTabs

        TabButton {
            id: tabs
            text: modelData.text
            background: Rectangle {
                color: tabs.pressed ? Theme.pressColor : (tabs.hovered ? Theme.hoverColor : "transparent")
                radius: Theme.radius
            }
            contentItem: Text {
                text: tabs.text
                font.pixelSize: Theme.fontSizeBody
                font.family: Theme.fontFamily
                color: Theme.textColor
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        } // ToolButton
    } // Repeater
}
