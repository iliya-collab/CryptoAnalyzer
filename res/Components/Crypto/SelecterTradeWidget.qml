import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Application.Core 1.0
import Theme 1.0
import Components.Custom 1.0

Item {
    id: root

    readonly property var tradePairList: TradePairsFilterProxyModel {
        sourceModel: AppCore.marketState.tradePairs
        quoteCoinFilter: ""
    }

    property alias popupWidth: mainPopup.width
    property alias popupHeight: mainPopup.height
    readonly property alias currentTrade: lblTitle.text

    implicitHeight: lblTitle.implicitHeight + (lblTitle.anchors.margins * 2)
    implicitWidth: lblTitle.implicitWidth + (lblTitle.anchors.margins * 2)
    opacity: hoverHandler.hovered ? 0.8 : 1.0

    signal itemSelected(string item)

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
        text: (lstTrades.model && lstTrades.model.length > 0) ? "Select..." : "None"
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
                    var curText = modelTabs[currentIndex].text
                    if (root.isComponentReady)
                    {
                        tradePairList.quoteCoinFilter = curText === "ALL" ? "" : curText
                    }
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
                    model: tradePairList
                    anchors.fill: parent
                    clip: true
                    delegate: ItemDelegate {
                        id: lstItem
                        text: symbol
                        background: Rectangle { color: lstItem.pressed ? Theme.pressColor : (lstItem.hovered ? Theme.hoverColor : "transparent") }
                        contentItem: Text {
                            text: parent.text
                            font.pixelSize: Theme.fontSizeBody
                            font.family: Theme.fontFamily
                            padding: Theme.padding
                            color: Theme.textColor
                        }
                        onClicked: {
                            lblTitle.text = symbol
                            root.itemSelected(symbol)
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