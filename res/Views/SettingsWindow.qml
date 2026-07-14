import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Theme 1.0

Window {
    id: root
    width: 600
    height: 400
    visible: true
    title: "Settings"
    color: Theme.windowColor

    ListModel {
        id: settingsModel
        ListElement { type: "header"; text: "Account" }
        ListElement { type: "item"; text: "API"; clicked: function() { contentStack.showAPIPage() } }
    }

    RowLayout {
        id: contentLayout
        anchors.fill: parent
        Rectangle {
            Layout.preferredWidth: parent.width / 3
            Layout.fillHeight: true
            color: "transparent"
            border.width: Theme.borderWidth
            border.color: Theme.borderColor
            ListView {
                id: settingsList
                anchors.fill: parent
                anchors.margins: Theme.margins
                spacing: Theme.spacing
                model: settingsModel

                delegate: Loader {
                    readonly property var itemData: model
                    width: settingsList.width

                    sourceComponent: {
                        if (itemData.type === "header")
                            return headerComponent
                        else if (itemData.type === "item")
                            return itemComponent
                    }
                    onLoaded: {
                        if (!item)
                            return
                        if (itemData.type === "header")
                            item.headerText = itemData.text
                        else if (itemData.type === "item") {
                            item.itemText = itemData.text
                            item.onClickCallback = itemData.clicked
                        }
                    }
                } // Loader
            } // settingsList
        } // Rectangle

        StackView {
            id: contentStack
            Layout.fillHeight: true
            Layout.fillWidth: true

            initialItem: Item {}

            // Метод для вызова экрана подключения API
            function showAPIPage() {
                contentStack.replace("APIScreen.qml")
            }
        } // contentStack
    } // contentLayout


// =============================    Шаблоны пунктов меню настройки  =============================

    Component {
        id: headerComponent
        Rectangle {
            property string headerText: ""

            width: parent.width
            height: txtHeader.paintedHeight
            color: "transparent"

            Text {
                id: txtHeader
                text: headerText
                font.family: Theme.fontFamily
                font.pixelSize: Theme.fontSizeBody
                font.bold: true
                color: Theme.textColor
                verticalAlignment: Text.AlignVCenter
            }
        }
    }

    Component {
        id: itemComponent
        Rectangle {
            property string itemText: ""
            property var onClickCallback: null

            width: parent.width
            height: btnItem.height
            color: "transparent"

            Button {
                id: btnItem
                anchors.verticalCenter: parent.verticalCenter
                anchors.fill: parent
                anchors.leftMargin: Theme.margins

                contentItem: Text {
                    text: itemText
                    font.family: Theme.fontFamily
                    font.pixelSize: Theme.fontSizeBody
                    color: Theme.textColor
                }

                background: Rectangle {
                    color: btnItem.pressed ? Theme.pressColor : ( (btnItem.hovered && btnItem.enabled) ? Theme.hoverColor : "transparent" )
                }

                onClicked: {
                    if (onClickCallback)
                        onClickCallback()
                }
            }
        }
    }

} // settingsWindow
