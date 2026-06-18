import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Theme 1.0

ToolBar {
    id: root

    property var modelToolButtons: []
    signal toolButtonClicked(string id, string button)

    background: Rectangle { color: Theme.toolBarColor }

    RowLayout {
        anchors.fill: parent
        spacing: Theme.spacing

        Repeater {
            model: root.modelToolButtons

            ToolButton {
                id: btnMenu
                text: modelData.text
                background: Rectangle { color: btnMenu.pressed ? Theme.pressColor : (btnMenu.hovered ? Theme.hoverColor : "transparent") }
                contentItem: RowLayout {
                    anchors.centerIn: parent

                    Image {
                        visible: !!modelData.icon
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                        source: modelData.icon
                        sourceSize: Qt.size(24, 24)
                        fillMode: Image.PreserveAspectFit
                    }

                    CustomText {
                        visible: btnMenu.text !== ""
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                        text: btnMenu.text
                        font.pixelSize: Theme.fontSizeSmall
                    }
                }

                onClicked: root.toolButtonClicked(modelData.id, btnMenu.text)
            } // ToolButton
        } // Repeater

        Item { Layout.fillWidth: true }
    } // RowLayout
} // CustomToolBar
