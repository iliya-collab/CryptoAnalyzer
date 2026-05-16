import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root

    property alias titleItem: lblTitle.text
    property alias contentItem: lblContent.text

    color: "transparent"
    visible: true

    // Автоматический расчет высоты на основе содержимого
    implicitHeight: contentLayout.implicitHeight + (contentLayout.anchors.margins * 2)
    implicitWidth: contentLayout.implicitWidth + (contentLayout.anchors.margins * 2)

    ColumnLayout {
        id: contentLayout
        anchors.fill: parent

        spacing: 5
        anchors.margins: 5

        Label {
            id: lblTitle
            text: titleItem
            font.pixelSize: 12
            color: theme.textColor
            horizontalAlignment: Text.AlignHCenter
            Layout.fillWidth: true
        }

        Label {
            id: lblContent
            text: contentItem
            font.pixelSize: 12
            color: theme.textColor
            horizontalAlignment: Text.AlignHCenter
            Layout.fillWidth: true
        }

    }
}
