import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Theme 1.0

Rectangle {
    id: root

    property alias text: statusText.text
    property alias progressValue: progressBar.value
    property bool autoHide: true
    property alias durationHide: hideAnimation.duration
    property alias intervalHide: hideTimer.interval

    // Значения по умолчанию
    implicitWidth: contantLayout.width
    implicitHeight: contantLayout.height
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

    Timer {
        id: hideTimer
        interval: 1000
        running: false
        onTriggered: root.visible = false
    }

    onProgressValueChanged: {
        if (autoHide && progressValue >= progressBar.to) {
            hideTimer.start()
        }
    }

    RowLayout {
        id: contantLayout
        spacing: Theme.spacing
        Label {
            id: statusText
            Layout.leftMargin: Theme.margins
            text: "Ready"
            font.pixelSize: Theme.fontSizeSmall
            font.family: Theme.fontFamily
            color: Theme.textColor
        }
        ProgressBar {
            id: progressBar
            from: 0
            to: 100
            value: 0
        }
    }

    // Кликабельная область
    MouseArea {
        anchors.fill: parent
        onClicked: {
            if (progressValue >= progressBar.to)
                root.visible = false
        }
    }
}