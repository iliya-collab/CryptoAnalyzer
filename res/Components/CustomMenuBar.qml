import QtQuick
import QtQuick.Controls

import Theme 1.0

MenuBar {
    id: root
    property var menuModel: []
    signal itemTriggered(string menuTitle, string itemText)

    background: Rectangle { color: Theme.menuBarColor }

    delegate: MenuBarItem {
        id: menuBarItem

        background: Rectangle { color: menuBarItem.pressed ? Theme.pressColor : (menuBarItem.hovered ? Theme.hoverColor : "transparent") }

        contentItem: Text {
            text: menuBarItem.text
            font.pixelSize: Theme.fontSizeBody
            font.family: Theme.fontFamily
            color: Theme.textColor
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }

    Component.onCompleted: {
        for (var i = 0; i < menuModel.length; i++) {
            var item = menuModel[i];

            if (item.items && Array.isArray(item.items) && item.items.length > 0) {
                // Создаем Menu с выпадающим списком
                var menu = menuWithItemsComponent.createObject(root, {
                    title: item.title,
                    items: item.items
                });
                addMenu(menu);
            } else if (item.title) {
                // Создаем Menu без выпадающего списка (просто кнопка)
                var actionMenu = menuWithoutItemsComponent.createObject(root, {
                    title: item.title
                });
                addMenu(actionMenu);
            }
        }
    }

    // Компонент для меню с элементами
    Component {
        id: menuWithItemsComponent

        Menu {
            id: subMenu
            property string menuTitle: title
            property var items: []
            title: menuTitle

            background: Rectangle {
                color: Theme.menuBarColor
                border.width: Theme.borderWidth
                border.color: Theme.borderColor
                radius: Theme.radius
            }

            Repeater {
                model: items

                delegate: MenuItem {
                    id: menuItem

                    background: Rectangle {
                        color: menuItem.pressed ? Theme.pressColor : (menuItem.hovered ? Theme.hoverColor : Theme.menuBarColor)
                    }

                    contentItem: Item {
                        implicitWidth: lblText.implicitWidth + lblArrow.implicitWidth
                        implicitHeight: Math.max(lblText.implicitHeight, lblArrow.implicitHeight)

                        Text {
                            id: lblText
                            text: modelData.text
                            font.pixelSize: Theme.fontSizeBody
                            font.family: Theme.fontFamily
                            color: Theme.textColor
                            anchors.left: parent.left
                            anchors.leftMargin: Theme.margins
                            anchors.verticalCenter: parent.verticalCenter
                        }

                        Label {
                            id: lblArrow
                            text: "\u203A"
                            font.pixelSize: Theme.fontSizeSmall
                            font.family: Theme.fontFamily
                            font.bold: true
                            color: Theme.textColor
                            anchors.right: parent.right
                            anchors.rightMargin: Theme.margins
                            anchors.verticalCenter: parent.verticalCenter
                        }
                    }

                    onTriggered: root.itemTriggered(menuTitle, modelData.text)
                }
            }
        }
    }

    // Компонент для меню без элементов (просто кнопка)
    Component {
        id: menuWithoutItemsComponent

        Menu {
            id: emptyMenu
            property string menuTitle: title
            title: menuTitle

            closePolicy: Popup.NoAutoClose

            visible: false

            onAboutToShow: {
                close()
                root.itemTriggered(menuTitle, "")
            }

            background: Rectangle {
                color: "transparent"
            }
        }
    }
}