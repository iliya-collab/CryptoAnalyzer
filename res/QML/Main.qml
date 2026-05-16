import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import Engine 1.0
import MyApp 1.0
import "."
import "Components/TradePageWidget"
import "Components/CustomWidgets"

// Главное окно
ApplicationWindow {

    id: mainWindow
    visible: true
    width: 800
    height: 600
    title: "Bybit platform"

    color: theme.windowColor

    // ДВИЖОК ПРИЛОЖЕНИЯ
    ApplicationEngine {
        id: appEngine

        // Сигналы из движка
        // Обрабатываем прогресс загрузки
        onLoadingProgress: function(step, current, total) {
            statusWidget.progressValue = current / total * 100
            statusWidget.text = step + " - completed " + statusWidget.progressValue + "%"
        }

        // Обрабатываем ошибки
        onErrorOccurred: function(error) {
            statusWidget.text = error
        }

        // Обрабатываем загрузку TradePageWidgetузку
        onLoadingFinished: function(success) {
            statusWidget.text = (success) ? "Loading completed!" : "Loading failed!"
        }

        // Дополнительные сигналы
        // Добавляем сигнал для запуска движка из UI
        signal runEngine(string action)

        // Обработка runEngine
        onRunEngine: function(action) {
            if (action === "Spot")
                appEngine.run(Engine.WebSocketEndpoints.SPOT)
            else if (action === "Linear")
                appEngine.run(Engine.WebSocketEndpoints.LINEAR)
            else if (action === "Inverse")
                appEngine.run(Engine.WebSocketEndpoints.INVERSE)
            else if (action === "Option")
                appEngine.run(Engine.WebSocketEndpoints.OPTION)
        }

    } // ApplicationEngine

    // Меню
    menuBar : MenuBar {

        background: Rectangle { color: theme.muneBarColor }

        Menu {
            title: "Trade"

            background: Rectangle { color: theme.muneBarColor }

            MenuItem {
                text: "Spot"

                Label {
                   text: "\u203A"
                   font.bold: true
                   anchors.right: parent.right
                   anchors.rightMargin: 10
                   anchors.verticalCenter: parent.verticalCenter
                   color: theme.textColor
               }

                onTriggered:{
                    mainStack.currentIndex = tradePage.StackLayout.index
                    appEngine.runEngine("Spot");
                }
            }

            MenuItem {
                text: "Linear"

                Label {
                   text: "\u203A"
                   font.bold: true
                   anchors.right: parent.right
                   anchors.rightMargin: 10
                   anchors.verticalCenter: parent.verticalCenter
                   color: theme.textColor
               }

                onTriggered:{
                    mainStack.currentIndex = tradePage.StackLayout.index
                    appEngine.runEngine("Linear");
                }
            }

            MenuItem {
                text: "Inverse"

                Label {
                   text: "\u203A"
                   font.bold: true
                   anchors.right: parent.right
                   anchors.rightMargin: 10
                   anchors.verticalCenter: parent.verticalCenter
                   color: theme.textColor
               }

                onTriggered:{
                    mainStack.currentIndex = tradePage.StackLayout.index
                    appEngine.runEngine("Inverse");
                }
            }

            MenuItem {
                text: "Option"

                Label {
                   text: "\u203A"
                   font.bold: true
                   anchors.right: parent.right
                   anchors.rightMargin: 10
                   anchors.verticalCenter: parent.verticalCenter
                   color: theme.textColor
               }

                onTriggered:{
                    mainStack.currentIndex = tradePage.StackLayout.index
                    appEngine.runEngine("Option");
                }
            }

        } // Menu "Trade"
    } // MenuBar

    // Панель инструментов
    header: ToolBar {

        background: Rectangle { color: theme.toolBarColor }

        RowLayout {
            anchors.fill: parent
            spacing: 0

            ToolButton {
                text: "☰"
                onClicked: drawer.opened ? drawer.close() : drawer.open()
            }
        }
    }

    // Левая панель
    Drawer {
        id: drawer
        width: parent.width / 6
        height: parent.height
        edge: Qt.LeftEdge
        modal: true

        Rectangle {
            anchors.fill: parent
            color: theme.toolBarColor

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20

                Text {
                    text: "Main menu"
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    color: theme.textColor
                    font.pixelSize: 20
                    font.bold: true
                    Layout.bottomMargin: 10
                }

                // Разделитель
                Rectangle {
                    height: 1;
                    Layout.fillWidth: true;
                    color: theme.textColor
                    Layout.bottomMargin: 10
                }

                Button {
                    id: btnSettings
                    text: "Settings"
                    Layout.fillWidth: true
                    implicitHeight: 40

                    background: Rectangle {
                        implicitWidth: 100
                        implicitHeight: 40
                        color: theme.buttonColor
                        border.color: theme.borderColor
                        border.width: 2
                    }

                    contentItem: Text {
                        text: btnSettings.text
                        font: btnSettings.font
                        color: theme.textColor
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    onClicked: {
                        drawer.close()

                        if ((windowLoader.status === Loader.Ready) && windowLoader.item)
                            windowLoader.item.show()
                        else
                            windowLoader.source = "Components/Windows/SettingsWindow.qml"

                    }
                }

                Item { Layout.fillHeight: true }

                Button {
                    id: btnHelp
                    text: "Help"
                    Layout.fillWidth: true

                    background: Rectangle {
                        implicitWidth: 100
                        implicitHeight: 40
                        color: theme.buttonColor
                        border.color: theme.borderColor
                        border.width: 2
                    }

                    contentItem: Text {
                        text: btnHelp.text
                        font: btnHelp.font
                        color: theme.textColor
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    onClicked: drawer.close()
                }
            } // ColumnLayout
        } // Rectangle
    } // Drawer

    // Главное окно
    ColumnLayout {
        id: mainLayout
        anchors.fill: parent
        spacing: 0

        StackLayout {
            id: mainStack
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: 0

            Rectangle {
                id: homePage
                color: "transparent"
            }

            TradePage {
                id: tradePage
                cmbModel: appEngine.tradeList
            }
        } // mainStack

        // Progress bar и status bar
        CustomStatusWidget {
            id: statusWidget

            Layout.alignment: Qt.AlignRight | Qt.AlignBottom
            Layout.margins: 8

            // Настройка по умолчанию
            width: 300
            height: 80
            text: "Ready"

            durationHide: 3000
            intervalHide: 1000
        } // statusWidget

    } // mainLayout

    Component.onCompleted: console.log("UI Loaded successfully")

    Loader { id: windowLoader }

    Theme { id: theme }

}
