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

        Engine.loadingProgress.connect(function(step, current, total) {
            statusWidget.progressValue = current / total * 100
            statusWidget.text = step + " - completed " + statusWidget.progressValue + "%"
        })

        Engine.errorOccurred.connect(function(error) {
            statusWidget.text = error
        })

        Engine.loadingFinished.connect(function(success) {
            statusWidget.text = (success) ? "Loading completed!" : "Loading failed!"
        })

        Engine.engineStarted.connect(function() {
            Engine.loadTradingPairs("ALL")
        })

        Engine.apiChecked.connect(function(isValid) {
            if (isValid)
                console.log("API is valid");
            else
                console.log("API is not valid");
        })
    }

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

    // Меню
    menuBar: CustomMenuBar {
        menuModel: [
            {
                title: "Account",
                items: [
                    { text: "Connect API" }
                ]
            },
            {
                title: "Trade",
                items: [
                    { text: "Spot" }
                ]
            },
            {
                title: "View",
                items: [
                    {
                        text: "Orderbook",
                        checkable: true,
                        checked: mainWindow.visibleOrderbook
                    }
                ]
            },
            {
                title: "Settings"
            }
        ]

        onItemTriggered: function(menuTitle, itemText, checkedState) {
            if (itemText === "Connect API")
                mainStack.showConnectAPI()
            else if (itemText === "Orderbook") {
                mainWindow.visibleOrderbook = checkedState
                if (mainStack.currentItem && typeof mainStack.currentItem.setOrderbookVisible === "function")
                    mainStack.currentItem.setOrderbookVisible(mainWindow.visibleOrderbook)
            }
            else if (itemText === "Spot")
                mainStack.showTradePage()
        }
    }

    // Главное окно
    ColumnLayout {
        id: mainLayout
        anchors.fill: parent

        StackView {
            id: mainStack
            Layout.fillHeight: true
            Layout.fillWidth: true

            // Стартовый пустой экран
            initialItem: Item {}

            onCurrentItemChanged: {
                if (mainStack.currentItem && typeof mainStack.currentItem.setOrderbookVisible === "function")
                    mainStack.currentItem.setOrderbookVisible(mainWindow.visibleOrderbook)
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
    } // mainLayout

    footer: Rectangle {
        border.color: Theme.borderColor
        border.width: Theme.borderWidth
        color: Theme.windowColor
        height: 30

        RowLayout {
            anchors.fill: parent
            CustomStatusBar {
                id: statusWidget
                Layout.fillHeight: true
                Layout.fillWidth: true
                autoHide: false
            }
        }
    }

    Loader { id: windowLoader }

}
