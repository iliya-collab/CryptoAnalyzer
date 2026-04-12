import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    property string title: "Item"
    property var subMenuItems: []
    property bool subMenuVisible: false

    width: 80
    height: 30
    color: hoverArea.containsMouse ? palette.button : palette.window
    border.color: palette.mid
    radius: 4

    signal subMenuItemClicked(string item)

    Text {
        text: root.title
        color: palette.buttonText
        anchors.centerIn: parent
        font.pointSize: 10
    }

    MouseArea {
        id: hoverArea
        anchors.fill: parent
        hoverEnabled: true
        onEntered: {
            root.subMenuVisible = true
            closeTimer.stop()
        }
        onExited: closeTimer.start()
    }

    Popup {
        id: popup
        parent: root.parent
        x: root.x
        y: root.y + root.height
        width: root.width
        height: subMenuColumn.implicitHeight + 8
        padding: 4
        visible: root.subMenuVisible
        closePolicy: Popup.NoAutoClose

        background: Rectangle {
            color: palette.window
            border.color: palette.mid
            radius: 4
        }

        ColumnLayout {
            id: subMenuColumn
            spacing: 2
            anchors.fill: parent

            Repeater {
                model: root.subMenuItems
                delegate: Rectangle {
                    width: popup.width
                    height: 24
                    color: mouseArea.containsMouse ? palette.button : "transparent"
                    Layout.fillWidth: true

                    Text {
                        text: modelData
                        color: palette.buttonText

                        // Выравнивание текста внутри области Text
                        horizontalAlignment: Text.AlignLeft

                        // Привязка самого элемента Text (вместо anchors.centerIn)
                        anchors.left: parent.left
                        anchors.verticalCenter: parent.verticalCenter

                        font.pointSize: 9
                    }

                    MouseArea {
                        id: mouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            root.subMenuItemClicked(modelData)
                            root.subMenuVisible = false
                        }
                        onEntered: closeTimer.stop()
                        onExited: closeTimer.start()
                    }
                }
            }
        }
    }

    Timer {
        id: closeTimer
        interval: 150
        onTriggered: {
            if (!hoverArea.containsMouse && (!popup.visible || !popup.containsMouse))
                root.subMenuVisible = false
        }
    }
}