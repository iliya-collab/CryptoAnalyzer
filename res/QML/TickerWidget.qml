import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    Layout.fillWidth: true

    color: "transparent"
    visible: true
    border.width: 1
    border.color: sysPal.midlight

    implicitHeight: contentLayout.implicitHeight + (contentLayout.anchors.margins * 2)
    implicitWidth: contentLayout.implicitWidth + (contentLayout.anchors.margins * 2)

    property ListModel tickerData: ListModel {
        ListElement { title: "Price"; value: "---" }
        ListElement { title: "Changes, 24h"; value: "---" }
        ListElement { title: "Max, 24h"; value: "---" }
        ListElement { title: "Min, 24h"; value: "---" }
        ListElement { title: "Volume, 24h"; value: "---" }
        ListElement { title: "Annual interest rate"; value: "---" }
    }

    RowLayout {
        id: contentLayout
        anchors.fill: parent
        anchors.margins: 5
        spacing: 10

        Repeater {
            model: root.tickerData

            TickerItem {
                titleItem: model.title
                contentItem: model.value
                Layout.fillWidth: true
                Layout.minimumWidth: 70
            }
        } // Repeater
    } // contentLayout

} // TickerWidget