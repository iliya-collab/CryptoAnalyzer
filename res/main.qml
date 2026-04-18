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

    // Фиксируем размер
    minimumWidth: width
    maximumWidth: width
    minimumHeight: height
    maximumHeight: height

    // Убираем кнопку "Развернуть" из заголовка
    flags: Qt.Window | Qt.WindowCloseButtonHint | Qt.WindowMinimizeButtonHint

    // ДВИЖОК ПРИЛОЖЕНИЯ
    ApplicationEngine {
        id: appEngine

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

            appEngine.startTrade()
        }

    } // ApplicationEngine

    header: ToolBar {
        RowLayout {
            anchors.fill: parent
            spacing: 0

            ToolButton {
                id: mainMenu
                text: "☰"
                onClicked: {
                    drawer.opened ? drawer.close() : drawer.open()
                }
            } // mainMenu

            ToolButton {
                id: tradeMenu
                text: "Trade"

                onClicked: {
                    drawer.close()
                    tradeMenuPopup.open()
                }

                Popup {
                    id: tradeMenuPopup
                    y: parent.height
                    width: 150
                    modal: true
                    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

                    Column {
                        width: parent.width

                        Button {
                            text: "Spot"
                            width: parent.width
                            onClicked: {
                                appEngine.runEngine("Spot")
                                tradeMenuPopup.close()
                            }
                        }

                        Button {
                            text: "Linear"
                            width: parent.width
                            onClicked: {
                                appEngine.runEngine("Linear")
                                tradeMenuPopup.close()
                            }
                        }

                        Button {
                            text: "Inverse"
                            width: parent.width
                            onClicked: {
                                appEngine.runEngine("Inverse")
                                tradeMenuPopup.close()
                            }                    x: 0
                        }

                        Button {
                            text: "Option"
                            width: parent.width
                            onClicked: {
                                appEngine.runEngine("Option")
                                tradeMenuPopup.close()
                            }
                        }
                    }
                } // tradeMenuPopup
            } // tradeMenu

            ToolButton {
                id: availableMenu
                text: "Available"

                onClicked: {
                    drawer.close()
                    availablePopup.open()
                }

                Popup {
                    id: availablePopup
                    y: parent.height
                    width: 220
                    height: 300
                    modal: true
                    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

                    Column {
                        anchors.fill: parent
                        anchors.margins: 10
                        spacing: 10

                        Rectangle {
                            width: parent.width
                            height: parent.height
                            border.color: sysPal.midlight   // Цвет рамки
                            border.width: 2                 // Толщина рамки
                            color: "transparent"            // Прозрачный фон (или можно задать свой)
                            radius: 4                       // Скругление углов (опционально)

                            ListView {
                                id: listView
                                anchors.fill: parent
                                // Если скроллбар виден, добавляем отступ справа, равный его ширине
                                rightMargin: vScrollBar.visible ? vScrollBar.width : 0
                                // Аналогично для горизонтального (чтобы текст не уходил под него)
                                bottomMargin: hScrollBar.visible ? hScrollBar.height : 0

                                model: appEngine.tradeList
                                clip: true

                                delegate: ItemDelegate {
                                    text: modelData
                                    width: ListView.view.availableWidth
                                    onClicked: {
                                        console.log(model.name + " clicked")
                                        availablePopup.close()
                                    }
                                }

                                contentWidth: {
                                    var maxWidth = 0
                                    for (var i = 0; i < model.length; i++) {
                                        var textWidth = model[i].length * 8 + 20
                                        maxWidth = Math.max(maxWidth, textWidth)
                                    }
                                    return Math.max(maxWidth, width)
                                }

                                ScrollBar.vertical: ScrollBar {
                                    id: vScrollBar
                                    policy: ScrollBar.AsNeeded
                                }
                                ScrollBar.horizontal: ScrollBar {
                                    id: hScrollBar
                                    policy: ScrollBar.AsNeeded
                                }

                            } // listView
                        }
                    }
                } // availablePopup
            } // availableMenu

            // Растягиваем остальное пространство
            Item { Layout.fillWidth: true }
        }
    }

    // Левая панель
    Drawer {
        id: drawer
        width: 150
        height: parent.height
        edge: Qt.LeftEdge
        modal: true

        Rectangle {
            anchors.fill: parent
            color: palette.window

            ColumnLayout {
                anchors.fill: parent // Растягиваем Layout по всей высоте
                anchors.margins: 20
                spacing: 0  // Убираем расстояние между элементами

                Text {
                    text: "Main menu"
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
                        console.log("Settings")
                    }
                }

                // Элемент-распорка забирает оставшуюся высоту
                Item { Layout.fillHeight: true }

                Button {
                    text: "Help"
                    Layout.fillWidth: true

                    onClicked: {
                        drawer.close()
                        console.log("About the program")
                    }
                }
            } // ColumnLayout
        } // Rectangle
    } // Drawer

    // Progress bar и status bar
    CustomStatusWidget {
        id: statusWidget
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.margins: 8

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

    Component.onCompleted: {
        console.log("UI Loaded successfully")
    }

    SystemPalette {
        id: sysPal
    }

}
