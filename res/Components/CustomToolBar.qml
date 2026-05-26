import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Theme 1.0

ToolBar {
    id: root

    property var modelToolButtons: []
    signal toolButtonClicked(string button)

    background: Rectangle { color: Theme.toolBarColor }

    RowLayout {
        anchors.fill: parent

        Repeater {
            model: root.modelToolButtons

            ToolButton {
                id: btnMenu
                text: modelData.text
                background: Rectangle { color: btnMenu.pressed ? Theme.pressColor : (btnMenu.hovered ? Theme.hoverColor : "transparent") }
                contentItem: Text {
                    text: btnMenu.text
                    font.pixelSize: Theme.fontSizeBody
                    font.family: Theme.fontFamily
                    color: Theme.textColor
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: root.toolButtonClicked(btnMenu.text)
            } // ToolButton
        } // Repeater
    } // RowLayout
} // CustomToolBar
