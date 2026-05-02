import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root

    property alias popupList: lstTrades.model
    property alias popupWidth: mainPopup.width
    property alias popupHeight: mainPopup.height

    color: "transparent"
    visible: true
    border.width: 1
    border.color: sysPal.midlight

    // Автоматический расчет высоты на основе содержимого
    implicitHeight: lblTitle.implicitHeight + (lblTitle.anchors.margins * 2)
    implicitWidth: lblTitle.implicitWidth + (lblTitle.anchors.margins * 2)

    // Добавляем эффект при наведении
    opacity: hoverHandler.hovered ? 0.8 : 1.0

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
        font.pixelSize: 14
        font.bold: true
        color: sysPal.text

        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter

        anchors.fill: parent
        anchors.margins: 5
    }

    // Popup, который появляется при наведении
    Popup {
        id: mainPopup
        y: root.height
        width: popupWidth
        height: popupHeight
        modal: false
        focus: false
        closePolicy: Popup.NoAutoClose

        HoverHandler {
            id: popupHoverHandler
            onHoveredChanged: {
                if (!hovered) {
                    // Если курсор ушел с popup и с root, запускаем таймер закрытия
                    if (!hoverHandler.hovered)
                        popupTimer.restart()
                } else {
                    // Курсор на popup - останавливаем таймер
                    popupTimer.stop()
                }
            }
        }

        ColumnLayout {
            id: contentLayout
            anchors.fill: parent
            anchors.margins: 10

            RowLayout {
                Layout.fillWidth: true
                anchors.margins: 10

                ButtonGroup {
                    id: btnGroup
                    onClicked: {
                        appEngine.runEngine(button.text)
                        //console.log("Selected:", button.text)
                    }
                }

                Button {
                    text: "Spot"
                    ButtonGroup.group: btnGroup
                }
                Button {
                    text: "Linear"
                    ButtonGroup.group: btnGroup
                }
                Button {
                    text: "Inverse"
                    ButtonGroup.group: btnGroup
                }
                Button {
                    text: "Option"
                    ButtonGroup.group: btnGroup
                }
            } // RowLayout

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                border.color: sysPal.midlight   // Цвет рамки
                border.width: 2                 // Толщина рамки
                color: "transparent"            // Прозрачный фон (или можно задать свой)
                radius: 4                       // Скругление углов (опционально)

                ListView {
                    id: lstTrades
                    model: popupList
                    anchors.fill: parent
                    clip: true

                    onModelChanged: {
                        //console.log("ListView model changed:", model)
                        if (model && model.length > 0) {
                            //console.log("Model length:", model.length)
                            lblTitle.text = model[0]
                            appEngine.startTrade(lblTitle.text)
                        }
                    }

                    delegate: ItemDelegate {
                        text: modelData
                        width: ListView.view.availableWidth
                        onClicked: {
                            lblTitle.text = modelData
                            //console.log(lblTitle.text + " clicked")
                            appEngine.startTrade(lblTitle.text)
                            mainPopup.close()
                        }
                    }

                    ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }

                } // lstTrades
            } // Rectangle

        } // contentLayout

    } // mainPopup

} // SelecterTradeWidget