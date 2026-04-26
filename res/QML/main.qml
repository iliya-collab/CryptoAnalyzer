import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import Engine 1.0
import MyApp 1.0

// Главное окно
ApplicationWindow {

    // ПАРАМЕТРЫ ГЛАВНОГО ОКНА
    id: mainWindow
    visible: true
    width: 800
    height: 600
    title: "Bybit platform"

    /*// Фиксируем размер
    minimumWidth: width
    maximumWidth: width
    minimumHeight: height
    maximumHeight: height

    // Убираем кнопку "Развернуть" из заголовка
    flags: Qt.Window | Qt.WindowCloseButtonHint | Qt.WindowMinimizeButtonHint*/

    // ДВИЖОК ПРИЛОЖЕНИЯ
    ApplicationEngine {
        id: appEngine

        // Сигналы из движка
        // Обрабатываем прогресс загрузки
        onLoadingProgress: function(step, current, total) {
            if (statusWidget.progressTo !== total)
                statusWidget.progressTo = total

            statusWidget.progressValue = current
            statusWidget.text = step + " (" + current + "/" + total + ")"
        }

        // Обрабатываем ошибки
        onErrorOccurred: function(error) {
            statusWidget.text = "Error: " + error
        }

        // Обрабатываем загрузку
        onLoadingFinished: function(success) {
            if (success)
                statusWidget.text = "Loading completed!"
            else
                statusWidget.text = "Loading failed!"
        }

        // Дополнительные сигналы
        // Добавляем сигнал для запуска движка из UI
        signal runEngine(string action)

        // Обработка runEngine
        onRunEngine: function(action) {
            if (action === "Spot")
                trade = Engine.TypeTrade.SPOT
            else if (action === "Linear")
                trade = Engine.TypeTrade.LINEAR
            else if (action === "Inverse")
                trade = Engine.TypeTrade.INVERSE
            else if (action === "Option")
                trade = Engine.TypeTrade.OPTION

            appEngine.startEngine()
        }

    } // ApplicationEngine

    // Меню
    menuBar : MenuBar {
        Menu {
            title: "Trade"

            MenuItem {
                text: "Spot"

                Label {
                   text: "\u203A"
                   font.bold: true
                   anchors.right: parent.right
                   anchors.rightMargin: 10
                   anchors.verticalCenter: parent.verticalCenter
                   color: sysPal.midlight
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
                   color: sysPal.midlight
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
                   color: sysPal.midlight
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
                   color: sysPal.midlight
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
        RowLayout {
            anchors.fill: parent
            spacing: 0

            ToolButton {
                text: "☰"
                onClicked: {
                    drawer.opened ? drawer.close() : drawer.open()
                }
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
            color: palette.window

            ColumnLayout {
                anchors.fill: parent // Растягиваем Layout по всей высоте
                anchors.margins: 20

                Text {
                    text: "Main menu"
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter // Центрирует содержимое по горизонтали
                    verticalAlignment: Text.AlignVCenter   // Центрирует содержимое по вертикали
                    color: sysPal.text
                    font.pixelSize: 20
                    font.bold: true
                    Layout.bottomMargin: 10  // Добавляем отступ снизу
                }

                // Разделитель
                Rectangle {
                    height: 1;
                    Layout.fillWidth: true;
                    color: sysPal.midlight
                    Layout.bottomMargin: 10  // Отступ после разделителя
                }

                Button {
                    text: "Settings"
                    Layout.fillWidth: true

                    onClicked: {
                        drawer.close()
                        //console.log("Settings")
                    }
                }

                // Элемент-распорка забирает оставшуюся высоту
                Item { Layout.fillHeight: true }

                Button {
                    text: "Help"
                    Layout.fillWidth: true

                    onClicked: {
                        drawer.close()
                        //console.log("About the program")
                    }
                }
            } // ColumnLayout
        } // Rectangle
    } // Drawer

    // Главное окно
    ColumnLayout {
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
                Text {
                    text: "Главный экран";
                    anchors.centerIn: parent
                }
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
            progressValue: 50
            progressTo: 100

            durationHide: 3000
            onClicked: {
                console.log("Clicked on status bar")
            }

        } // statusWidget

    }

    Component.onCompleted: {
        console.log("UI Loaded successfully")
    }

    SystemPalette {
        id: sysPal
    }

}
