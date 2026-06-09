import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Theme 1.0

MenuBar {
    id: root
    property var menuModel: []
    signal itemTriggered(string menuTitle, string itemText, bool checked)

    background: Rectangle {
        color: Theme.menuBarColor
        border.color: Theme.borderColor
        border.width: Theme.borderWidth
    }

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

                    checkable: modelData.checkable !== undefined ? modelData.checkable : false
                    checked: modelData.checked !== undefined ? modelData.checked : false

                    indicator: Item {
                        implicitWidth: 0
                        implicitHeight: 0
                        visible: false
                    }

                    background: Rectangle {
                        color: menuItem.pressed ? Theme.pressColor : (menuItem.hovered ? Theme.hoverColor : Theme.menuBarColor)
                    }

                    contentItem: RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: Theme.margins
                        anchors.rightMargin: Theme.margins
                        spacing: Theme.margins

                        // Левая область: отображается только если элемент checkable
                        Rectangle {
                            id: customCheckBox
                            visible: menuItem.checkable
                            implicitWidth: 16
                            implicitHeight: 16
                            color: menuItem.checked ? Theme.pressColor : "transparent"
                            border.color: menuItem.checked ? Theme.pressColor : Theme.borderColor
                            border.width: Theme.borderWidth
                            Layout.alignment: Qt.AlignVCenter

                            // Текстовая галочка внутри квадрата
                            Text {
                                text: "\u2713"
                                font.pixelSize: Theme.fontSizeSmall
                                font.bold: true
                                color: Theme.textColor
                                visible: menuItem.checked
                                anchors.centerIn: parent
                            }
                        }

                        // Центральная область: Текст элемента
                        Text {
                            id: lblText
                            text: modelData.text
                            font.pixelSize: Theme.fontSizeBody
                            font.family: Theme.fontFamily
                            color: Theme.textColor
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignVCenter
                            elide: Text.ElideRight
                        }

                        // Правая область: Стрелочка (показывается только для обычных пунктов)
                        Text {
                            id: lblArrow
                            text: "\u203A"
                            font.pixelSize: Theme.fontSizeSmall
                            font.family: Theme.fontFamily
                            font.bold: true
                            color: Theme.textColor
                            visible: !menuItem.checkable
                            Layout.alignment: Qt.AlignVCenter
                        }
                    }

                    onTriggered: root.itemTriggered(menuTitle, modelData.text, menuItem.checked)
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
                root.itemTriggered(menuTitle, "", false)
            }

            background: Rectangle {
                color: "transparent"
            }
        }
    }
}