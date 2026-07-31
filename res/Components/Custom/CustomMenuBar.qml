import QtQuick
import QtQuick.Controls
import Theme 1.0

/*
    Пункт меню содержит следующие значения:
        * text - название пункта меню (если название будет '---', то данный пункт будет разделителем)
        * items - массив вложенных пунктов меню
        * enabled - определяет активен ли данный пункт
        * checkable - пункт меню с флажком
        * checked - начальное состояние пункта меню с флажком
        * clicked - функция обработчик, которая вызывается при клике на пункт меню
*/

MenuBar {
    id: root

    property var menuModel: []

    background: Rectangle {
        color: Theme.menuBarColor
        Behavior on color { ColorAnimation { duration: 100 } }
    }

    delegate: MenuBarItem {
        id: topMenuBarItem
        background: Rectangle {
            color: pressed && enabled ? Theme.pressColor :
                   (hovered && enabled ? Theme.hoverColor : "transparent")
            Behavior on color { ColorAnimation { duration: 100 } }
        }
        contentItem: Text {
            text: topMenuBarItem.text
            font.pixelSize: Theme.fontSizeBody
            font.family: Theme.fontFamily
            color: Theme.textColor
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }

    // Instantiator для меню верхнего уровня
    /*Instantiator {
        model: root.menuModel
        onObjectAdded: (index, object) => {
            if (object && object.item) {
                if (object.item.menuItems && object.item.menuItems.length > 0) {
                    //console.log("Top submenu - " + object.item.menuTitle + " added")
                    root.insertMenu(index, object.item)
                } else {
                    //console.log("Top menu item - " + object.item.itemText + " added")
                    root.insertItem(index, object.item)
                }
            }
        }
        onObjectRemoved: (index, object) => {
            if (object && object.item) {
                if (object.item.items && object.item.items.length > 0)
                    root.removeMenu(object)
                else
                    root.removeItem(object)
            }
        }
        delegate: Loader {
            property var itemData: modelData
            sourceComponent: {
                if (itemData.items && itemData.items.length > 0)
                    return submenuComponent
                else
                    return topMenuBarItemComponent
            }
            onLoaded: {
                if (!item)
                    return
                if (itemData.items && itemData.items.length > 0) {
                    //console.log("Top submenu - " + itemData.text + " loaded")
                    item.menuTitle = itemData.text
                    item.menuEnabled = itemData.enabled !== undefined ? itemData.enabled : true
                    item.menuItems = itemData.items || []
                    item.parentPath = ""
                } else if (itemData.text) {
                    //console.log("Top menu item - " + itemData.text + " loaded")
                    item.itemText = itemData.text
                    item.itemEnabled = itemData.enabled !== undefined ? itemData.enabled : true
                    item.itemShortcut = itemData.shortcut || ""
                    item.itemParentPath = ""
                }
            }
        } // Loader
    } // Instantiator*/

    property var menuItems: []

    // Очистка меню
    function clearMenu() {
        for (var i = menuItems.length - 1; i >= 0; i--) {
            var item = menuItems[i]
            if (item && item.parent === root) {
                if (item instanceof Menu) {
                    root.removeMenu(item)
                } else if (item instanceof MenuBarItem) {
                    root.removeItem(item)
                }
            }
        }
        menuItems = []
    }

    // Перестройка меню
    function rebuildMenu() {
        clearMenu()

        for (var i = 0; i < root.menuModel.length; i++) {
            var itemData = root.menuModel[i]
            var menuItem = createMenuItem(itemData)
            if (menuItem) {
                if (itemData.items && itemData.items.length > 0) {
                    root.insertMenu(i, menuItem)
                } else {
                    root.insertItem(i, menuItem)
                }
                menuItems.push(menuItem)
            }
        }
    }

    // Создание элемента меню
    function createMenuItem(data) {
        if (data.items && data.items.length > 0) {
            // Создаем подменю
            var menu = submenuComponent.createObject(root, {
                menuTitle: data.text,
                menuEnabled: data.enabled !== undefined ? data.enabled : true,
                menuItems: data.items || [],
            })
            return menu
        } else {
            // Создаем пункт меню
            var item = topMenuBarItemComponent.createObject(root, {
                itemText: data.text,
                itemEnabled: data.enabled !== undefined ? data.enabled : true,
                itemShortcut: data.shortcut || "",
                itemClicked: data.clicked || null
            })
            return item
        }
    }

    // Обработчик изменения модели
    onMenuModelChanged: {
        rebuildMenu()
    }

    // Компонент меню (без подпунктов)
    Component {
        id: menuItemComponent
        MenuItem {
            id: menuItem

            property alias itemText: menuItem.text
            property alias itemEnabled: menuItem.enabled
            property alias itemCheckable: menuItem.checkable
            property alias itemChecked: menuItem.checked
            property var itemClicked: null
            property string itemShortcut: ""

            //Component.onCompleted: console.log("Component menuItemComponent created")

            leftPadding: itemCheckable ? 32 : Theme.margins
            rightPadding: subMenu ? 32 : Theme.margins

            indicator: Rectangle {
                implicitWidth: 16
                implicitHeight: 16
                x: (leftPadding - width) / 2
                y: topPadding + (availableHeight - height) / 2
                color: itemChecked ? Theme.pressColor : "transparent"
                border.color: itemChecked ? Theme.pressColor : Theme.borderColor
                border.width: Theme.borderWidth
                visible: itemCheckable

                Text {
                    text: "\u2713"
                    font.pixelSize: Theme.fontSizeSmall
                    font.bold: true
                    color: Theme.textColor
                    visible: itemChecked
                    anchors.centerIn: parent
                }
            }

            contentItem: Text {
                text: menuItem.text
                font.pixelSize: Theme.fontSizeBody
                font.family: Theme.fontFamily
                color: enabled ? Theme.textColor : Theme.disabledColor
                verticalAlignment: Text.AlignVCenter
                width: availableWidth
                elide: Text.ElideRight
            }

            background: Rectangle {
                color: pressed && enabled ? Theme.pressColor :
                       (hovered && enabled ? Theme.hoverColor : "transparent")
                Behavior on color { ColorAnimation { duration: 100 } }
            }

            onCheckedChanged: {
                itemChecked = itemCheckable ? checked : false
            }

            onTriggered: {
                if (itemClicked)
                    itemClicked()
            }
        }
    } // menuItemComponent

    // Компонент меню (с подпунктоми)
    Component {
        id: submenuComponent
        Menu {
            id: submenu

            property alias menuTitle: submenu.title
            property alias menuEnabled: submenu.enabled
            property var menuItems: []

            //Component.onCompleted: console.log("Component submenuComponent created")

            delegate: MenuItem {
                id: item

                leftPadding: Theme.margins
                rightPadding: 32

                contentItem: Text {
                    text: item.text
                    font.pixelSize: Theme.fontSizeBody
                    font.family: Theme.fontFamily
                    color: item.enabled ? Theme.textColor : Theme.disabledColor
                    verticalAlignment: Text.AlignVCenter
                    width: item.availableWidth
                    elide: Text.ElideRight
                }

                background: Rectangle {
                    color: item.pressed && item.enabled ? Theme.pressColor :
                           (item.hovered && item.enabled ? Theme.hoverColor : "transparent")
                    Behavior on color { ColorAnimation { duration: 100 } }
                }
            }

            background: Rectangle {
                color: Theme.menuBarColor
                border.width: Theme.borderWidth
                border.color: Theme.borderColor
                radius: Theme.radius
            }

            // Instantiator для вложенного меню
            Instantiator {
                model: submenu.menuItems
                onObjectAdded: (index, object) => {
                    if (object && object.item) {
                        if (object.item.menuItems && object.item.menuItems.length > 0) {
                            //console.log("Submenu - " + object.item.menuTitle + " added")
                            submenu.insertMenu(index, object.item)
                        } else {
                            //console.log("Menu item - " + object.item.itemText + " added")
                            submenu.insertItem(index, object.item)
                        }
                    }
                }
                onObjectRemoved: (index, object) => {
                    if (object && object.item) {
                        if (object.item.menuItems && object.item.menuItems.length > 0)
                            submenu.removeMenu(object.item)
                        else
                            submenu.removeItem(object.item)
                    }
                }
                delegate: Loader {
                    property var itemData: modelData
                    sourceComponent: {
                        if (itemData.text === "---")
                            return menuSeparatorComponent
                        else if (itemData.items && itemData.items.length > 0)
                            return submenuComponent
                        else
                            return menuItemComponent
                    }
                    onLoaded: {
                        if (!item)
                            return
                        if (itemData.items && itemData.items.length > 0) {
                            //console.log("Submenu - " + itemData.text + " loaded")
                            item.menuTitle = itemData.text
                            item.menuEnabled = itemData.enabled !== undefined ? itemData.enabled : true
                            item.menuItems = itemData.items || []
                        } else if (itemData.text !== "---") {
                            //console.log("Menu item - " + itemData.text + " loaded")
                            item.itemText = itemData.text
                            item.itemEnabled = itemData.enabled !== undefined ? itemData.enabled : true
                            item.itemCheckable = itemData.checkable !== undefined ? itemData.checkable : false
                            item.itemChecked = itemData.checked !== undefined ? itemData.checked : false
                            item.itemShortcut = itemData.shortcut || ""
                            item.itemClicked = itemData.clicked || null
                        }
                    }
                } // itemLoader
            } // Instantiator
        } // submenu
    } // submenuComponent

    // Компонент разделителя
    Component {
        id: menuSeparatorComponent
        MenuSeparator {
            //Component.onCompleted: console.log("Component menuSeparatorComponent created")
            implicitHeight: 1
            background: Rectangle { color: Theme.textColor }
        }
    } // menuSeparatorComponent

    // Простой компонент меню (кнопка)
    Component {
        id: topMenuBarItemComponent
        MenuBarItem {
            id: menuBarItem

            property string itemText: ""
            property bool itemEnabled: true
            property string itemShortcut: ""
            property var itemClicked: null

            //Component.onCompleted: console.log("Component topMenuBarItemComponent created")

            text: itemText
            enabled: itemEnabled

            background: Rectangle {
                color: menuBarItem.pressed && menuBarItem.enabled ? Theme.pressColor :
                       (menuBarItem.hovered && menuBarItem.enabled ? Theme.hoverColor : "transparent")
                Behavior on color { ColorAnimation { duration: 100 } }
            }

            contentItem: Text {
                text: menuBarItem.text
                font.pixelSize: Theme.fontSizeBody
                font.family: Theme.fontFamily
                color: Theme.textColor
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            onTriggered: {
                if (itemClicked)
                    itemClicked()
            }
        }
    } // menuBarItemComponent

}