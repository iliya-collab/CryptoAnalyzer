import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import Application.UI 1.0
import Application.Core 1.0
import Theme 1.0
import Components.Custom 1.0
import Components.Crypto 1.0

// Главное окно
ApplicationWindow {

    id: mainWindow
    visible: true
    width: 1000
    height: 800
    title: "EasyTrade"
    color: Theme.windowColor

    visibility: "FullScreen"

    Connections {
        target: AppCore

        function onErrorOccurred(error) { statusWidget.text = error }

        function onMessageReceived(msg) { statusWidget.text = msg }

        function onDownloadProgress(received, total) {
            if (total > 0)
                statusWidget.progressValue = received / total * 100
        }
    }

    Component.onCompleted: {
        // Инициализируем ядро приложения
        AppCore.init()
        // Загружаем пары для трейдинга
        AppCore.loadTradesFromRepository()
        // Проверям валидность API, запрашивая информацию об аккаунте
        AppCore.requestAccount()
    }

    // Меню
    menuBar: CustomMenuBar {
        menuModel: [
            {
                text: "Trade",
                items: [
                    {
                        text: "Repository",
                        items: [
                            { text: "Load spot pairs" },
                            { text: "Load candles" }
                        ]
                    },
                    { text: "---" },
                    { text: "Spot" }
                ]
            },
            {
                text: "View",
                items: [
                    { text: "Order Book" },
                    { text: "Recent Trades" }
                ]
            },
            { text: "Settings" }
        ]

        onItemTriggered: function(itemText, itemPath, checkedState) {
            //console.log("Item:", itemText, " Full path:", itemPath)

            if (itemText === "Settings") {
                if (!settingsWindowLoader.active)
                    settingsWindowLoader.active = true
            }
            else if (itemText === "Order Book") {
                if (mainStack.currentItem && typeof mainStack.currentItem.showOrderbook === "function")
                    mainStack.currentItem.showOrderbook()
            }
            else if (itemText === "Recent Trades") {
                if (mainStack.currentItem && typeof mainStack.currentItem.showRecentTrades === "function")
                    mainStack.currentItem.showRecentTrades()
            }
            else if (itemText === "Spot")
                mainStack.showTradePage()
            else if (itemPath === "Repository > Load spot pairs")
                AppCore.loadTradesFromNetwork()
            else if (itemPath === "Repository > Load candles") {}
        }
    }

    // Главное окно
    Page {
        id: mainPage
        anchors.fill: parent

        background: Rectangle { color: Theme.windowColor }

        header: CustomToolBar {
            modelToolButtons: [
                { id: "btn_run", icon: "qrc:/icons/icon_play.png" },
                { id: "btn_restart", icon: "qrc:/icons/icon_restart.png" },
                { id: "btn_stop", icon: "qrc:/icons/icon_stop.png" }
            ]
            onToolButtonClicked: function(id, name) {
                if (id === "btn_run")
                    AppCore.run()
                else if (id === "btn_restart")
                    AppCore.restart()
                else if (id === "btn_stop")
                    AppCore.interrupt()
            }
        }

        StackView {
            id: mainStack
            anchors.fill: parent

            initialItem: Item {}

            // Метод для вызова экрана торговли
            function showTradePage() {
                mainStack.replace("Views/TradeScreen.qml")
            }
        } // mainStack

        footer: Rectangle {
            color: Theme.toolBarColor
            height: 30

            RowLayout {
                anchors.fill: parent
                CustomStatusBar {
                    id: statusWidget
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    autoHide: false
                    //visibleProgressBar: false
                }
            }
        }

    } // mainLayout

    Loader {
        id: settingsWindowLoader
        active: false
        source: "Views/SettingsWindow.qml"

        Connections {
            target: settingsWindowLoader.item
            function onVisibleChanged() {
                if (settingsWindowLoader.item && !settingsWindowLoader.item.visible)
                    settingsWindowLoader.active = false
            }
        }
    }

    PingIndicator {
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: Theme.margins

        width: 30
        height: 30
        thickness: 2

        pingValue: AppCore.pingMs
    }
}
