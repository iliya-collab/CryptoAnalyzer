import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import Engine 1.0
import MainApplication 1.0
import Theme 1.0
import Components.Custom 1.0

// Главное окно
ApplicationWindow {

    id: mainWindow
    visible: true
    width: 1000
    height: 800
    title: "Trader"
    color: Theme.windowColor

    property bool visibleOrderbook: true

    Component.onCompleted: {
        console.log("UI Loaded successfully")

        Engine.errorOccurred.connect(function(error) {
            statusWidget.text = error
        })

        Engine.messageReceived.connect(function(msg) {
            statusWidget.text = msg
        })

        Engine.downloadProgress.connect(function(received, total) {
            if (total > 0)
                statusWidget.progressValue = received / total * 100
        })

        Engine.engineStarted.connect(function() {
            Engine.loadTradesFromRepository()
        })

        Engine.apiChecked.connect(function(isValid) {
            if (isValid)
                console.log("API is valid");
            else
                console.log("API is not valid");
        })

        Engine.init()
    }

    // Меню
    menuBar: CustomMenuBar {
        menuModel: [
            {
                text: "Account",
                items: [
                    { text: "Connect API" }
                ]
            },
            {
                text: "Trade",
                items: [
                    {
                        text: "Load",
                        items: [
                            { text: "Spot pairs" },
                            { text: "Candles" }
                        ]
                    },
                    { text: "---" },
                    { text: "Spot" }
                ]
            },
            {
                text: "View",
                items: [
                    { text: "Orderbook" }
                ]
            },
            { text: "Settings" }
        ]

        onItemTriggered: function(itemText, itemPath, checkedState) {
            //console.log("Item:", itemText, " Full path:", itemPath)

            if (itemText === "Connect API")
                mainStack.showConnectAPI()
            else if (itemText === "Orderbook") {
                mainWindow.visibleOrderbook = !mainWindow.visibleOrderbook
                if (mainStack.currentItem && typeof mainStack.currentItem.setOrderbookVisible === "function")
                    mainStack.currentItem.setOrderbookVisible(mainWindow.visibleOrderbook)
            }
            else if (itemText === "Spot")
                mainStack.showTradePage()
            else if (itemPath === "Load > Spot pairs")
                Engine.loadTradesFromNetwork()
            else if (itemPath === "Load > Candles") {}
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
                    Engine.run()
                else if (id === "btn_restart")
                    Engine.restart()
                else if (id === "btn_stop")
                    Engine.interrupt()
            }
        }

        StackView {
            id: mainStack
            anchors.fill: parent

            initialItem: Item {}

            onCurrentItemChanged: {
                if (mainStack.currentItem && typeof mainStack.currentItem.setOrderbookVisible === "function")
                    mainStack.currentItem.orderbookVisible = mainWindow.visibleOrderbook
            }

            // Метод для вызова экрана подключения API
            function showConnectAPI() {
                mainStack.replace("Views/ConnectAPIScreen.qml")
            }

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

}
