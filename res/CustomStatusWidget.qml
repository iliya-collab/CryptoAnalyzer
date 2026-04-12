import QtQuick
import QtQuick.Controls

Rectangle {
    id: root
    width: 260
    height: 60
    color: "transparent"

    property alias text: statusText.text
    property alias progressValue: progressBar.value
    property alias progressTo: progressBar.to
    property alias progressFrom: progressBar.from

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

    // Progress Bar
    ProgressBar {
        id: progressBar
        from: 0
        to: 100
        value: 0
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        width: 250
    }

    // Кликабельная область
    MouseArea {
        anchors.fill: parent
        onClicked: root.clicked()
    }
}