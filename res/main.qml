import QtQuick
import QtQuick.Controls

import Engine 1.0
import MyApp 1.0

// Главное окно
ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: "Bybit platform"

    // Настройка темы
    palette.window: "#2c2c2c"
    palette.windowText: "white"
    palette.button: "#3c3c3c"
    palette.buttonText: "white"

    // Движок приложения
    ApplicationEngine {
        id: appEngine

        onLoadingProgress: function(step, current, total) {
            if (statusWidget.progressTo !== total)
                statusWidget.progressTo = total

            statusWidget.progressValue = current
            statusWidget.text = step + " (" + current + "/" + total + ")"
        }

        onErrorOccurred: function(error) {
            statusWidget.text = "Error: " + error
        }

        onLoadingFinished: function(success) {
            if (success)
                statusWidget.text = "Loading completed!"
            else
                statusWidget.text = "Loading failed!"

        }
    }

    // Основная панель меню
    Row {
        id: menuBar
        spacing: 2
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 8

        CustomMenuItem {
            id: generalMenu
            title: "General"
            subMenuItems: ["Settings"]
        }

        CustomMenuItem {
            id: tradeMenu
            title: "Trade"
            subMenuItems: ["Spot", "Linear", "Inverse", "Options"]

            onSubMenuItemClicked: function(item) {
                if (item === "Spot") {
                    console.log("Clicked Spot")
                    appEngine.startTrade(Engine.TypesTrade.SPOT)
                }
                else if (item === "Linear") {
                    console.log("Clicked Linear")
                    appEngine.startTrade(Engine.TypesTrade.LINEAR)
                }
                else if (item === "Inverse") {
                    console.log("Clicked Inverse")
                    appEngine.startTrade(Engine.TypesTrade.INVERSE)
                }
                else if (item === "Options") {
                    console.log("Clicked Option")
                    appEngine.startTrade(Engine.TypesTrade.OPTIONS)
                }
            }
        }
    }

    // Progress bar и status bar
    CustomStatusWidget {
        id: statusWidget
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.margins: 8

        // Обновление через свойства
        text: "Ready"
        progressValue: 50
        progressTo: 100
    }
}
