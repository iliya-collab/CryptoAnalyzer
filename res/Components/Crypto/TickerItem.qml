import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Application.Core 1.0
import Theme 1.0
import Components.Custom 1.0

Rectangle {
    id: root

    property alias titleItem: lblTitle.text
    property alias contentItem: lblContent.text

    color: "transparent"

    // Автоматический расчет высоты на основе содержимого
    implicitHeight: contentLayout.implicitHeight + (contentLayout.anchors.margins * 2)
    implicitWidth: contentLayout.implicitWidth + (contentLayout.anchors.margins * 2)

    ColumnLayout {
        id: contentLayout
        anchors.fill: parent

        anchors.margins: Theme.margins

        Label {
            id: lblTitle
            text: titleItem
            font.pixelSize: Theme.fontSizeBody
            color: Theme.textColor
            horizontalAlignment: Text.AlignHCenter
            Layout.fillWidth: true
        }

        Label {
            id: lblContent
            text: contentItem
            font.pixelSize: Theme.fontSizeBody
            color: Theme.textColor
            horizontalAlignment: Text.AlignHCenter
            Layout.fillWidth: true
        }

    }
}
