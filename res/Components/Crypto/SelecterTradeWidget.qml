import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import Theme 1.0
import Components.Custom 1.0
import EngineComponents 1.0

Rectangle {
    id: root

    property alias popupList: lstTrades.model
    property alias popupWidth: mainPopup.width
    property alias popupHeight: mainPopup.height

    color: Theme.windowColor
    border.width: Theme.borderWidth
    border.color: Theme.borderColor
    radius: Theme.radius
    implicitHeight: lblTitle.implicitHeight + (lblTitle.anchors.margins * 2)
    implicitWidth: lblTitle.implicitWidth + (lblTitle.anchors.margins * 2)
    opacity: hoverHandler.hovered ? 0.8 : 1.0

    signal itemSelected(string item)
    signal filterSelected(string filter)

    property bool isComponentReady: false

    // Таймер для закрытия/открытия mainPopup
    Timer {
        id: popupTimer
        interval: 300
        repeat: false
        onTriggered: {
            if (!hoverHandler.hovered && !popupHoverHandler.hovered)
                mainPopup.close()
            else if (hoverHandler.hovered && !mainPopup.visible)
                mainPopup.open()
        }
    }

    // Обработчик наведения на root
    HoverHandler {
        id: hoverHandler
        onHoveredChanged: popupTimer.restart()
    }

    Label {
        id: lblTitle
        text: (lstTrades.model && lstTrades.model.length > 0) ? lstTrades.model[0] : "None"
        anchors.fill: parent
        anchors.margins: Theme.margins
        font.bold: true
        font.pixelSize: Theme.fontSizeBody
        font.family: Theme.fontFamily
        padding: Theme.padding
        color: Theme.textColor
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    // Popup, который появляется при наведении
    Popup {
        id: mainPopup
        y: root.height
        implicitWidth: contentLayout.implicitWidth + mainPopup.padding * 2
        implicitHeight: contentLayout.implicitHeight + mainPopup.padding * 2
        padding: Theme.padding
        modal: false
        focus: false
        closePolicy: Popup.NoAutoClose

        background: Rectangle {
            border.color: Theme.borderColor
            border.width: Theme.borderWidth
            color: Theme.menuBarColor
            radius: Theme.radius
        }

        HoverHandler {
            id: popupHoverHandler
            onHoveredChanged: popupTimer.restart()
        }

        contentItem: ColumnLayout {
            id: contentLayout
            anchors.fill: parent
            anchors.margins: Theme.spacing

            CustomTabBar {
                id: tabsBar
                modelTabs: [
                    {text: "ALL"},
                    {text: "USDT"},
                    {text: "USDC"},
                    {text: "USDE"}
                ]
                onCurrentIndexChanged: {
                    if (root.isComponentReady)
                        root.filterSelected(modelTabs[currentIndex].text)
                }
            }

            // Список трейдов
            Rectangle {
                Layout.fillWidth: true
                implicitHeight: 200 + tabsBar.height
                implicitWidth: tabsBar.width
                border.color: Theme.borderColor
                border.width: Theme.borderWidth
                color: "transparent"
                radius: Theme.radius
                ListView {
                    id: lstTrades
                    model: popupList
                    anchors.fill: parent
                    clip: true
                    onModelChanged: {
                        if (root.isComponentReady && model && model.length > 0)
                            root.itemSelected(model[0])
                    }
                    delegate: ItemDelegate {
                        id: lstItem
                        text: modelData
                        background: Rectangle { color: lstItem.pressed ? Theme.pressColor : (lstItem.hovered ? Theme.hoverColor : "transparent") }
                        contentItem: Text {
                            text: parent.text
                            font.pixelSize: Theme.fontSizeBody
                            font.family: Theme.fontFamily
                            padding: Theme.padding
                            color: Theme.textColor
                        }
                        onClicked: {
                            lblTitle.text = modelData
                            root.itemSelected(modelData)
                            mainPopup.close()
                        }
                    }
                    ScrollBar.vertical: ScrollBar {
                        id: scrollBar
                        policy: ScrollBar.AsNeeded
                        minimumSize: Theme.scrollBarMinSize
                        background: Rectangle {
                            color: Theme.scrollBarColor
                            border.color: Theme.borderColor
                            border.width: Theme.borderWidth
                            radius: Theme.radius
                        }
                        contentItem: Rectangle {
                            implicitWidth: Theme.scrollBarWidth
                            implicitHeight: Theme.scrollBarHeight
                            color: scrollBar.hovered ? Theme.hoverColor : Theme.sliderColor
                            border.color: Theme.borderColor
                            border.width: Theme.borderWidth
                            radius: Theme.radius
                        }
                    }
                } // lstTrades
            } // Rectangle
        } // contentLayout
    } // mainPopup

    Component.onCompleted: root.isComponentReady = true

} // SelecterTradeWidget