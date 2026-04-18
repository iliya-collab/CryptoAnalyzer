import QtQuick
import QtQuick.Controls

Rectangle {
    id: root

    property alias text: statusText.text
    property alias progressValue: progressBar.value
    property alias progressTo: progressBar.to
    property alias progressFrom: progressBar.from
    property bool autoHide: true
    property alias durationHide: hideAnimation.duration

    // Значения по умолчанию
    width: 260
    height: 60
    color: "transparent"
    visible: true

    // Анимация исчезновения
    Behavior on visible {
        SequentialAnimation {
            PropertyAnimation {
                id: hideAnimation
                target: root
                property: "opacity"
                to: 0
                duration: 1000
            }
            PropertyAction {
                target: root
                property: "visible"
                value: false
            }
        }
    }

    onProgressValueChanged: {
        if (autoHide && progressValue >= progressBar.to)
            root.visible = false
    }

    signal clicked

    // Текст статуса
    Text {
        id: statusText
        anchors.bottom: progressBar.top
        anchors.right: parent.right
        anchors.bottomMargin: 5
        color: "white"
        text: "Ready"
        font.pixelSize: 12
    }

    ProgressBar {
        id: progressBar
        from: 0
        to: 100
        value: 0
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        width: root.width
    }

    // Кликабельная область
    MouseArea {
        anchors.fill: parent
        onClicked: root.clicked()
    }
}