import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import Engine 1.0
import MainApplication 1.0
import Theme 1.0
import Components 1.0

// Главное окно
ApplicationWindow {

    id: mainWindow
    visible: true
    width: 800
    height: 600
    title: "Trader"
    color: Theme.windowColor

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

        Engine.apiChecked.connect(function(isValid) {
            if (isValid)
                console.log("API is valid");
            else
                console.log("API is not valid");
        })
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
                title: "Settings"
            },
            {
                title: "Trade",
                items: [
                    { text: "Spot" }
                ]
            }
        ]

        onItemTriggered: function(menuTitle, itemText, index) {
            if (itemText === "Connect API")
                mainStack.currentIndex = connectAPI.StackLayout.index
            else if (itemText === "Spot") {
                mainStack.currentIndex = tradePage.StackLayout.index
                Engine.run()
            }
        }
    }

    // Главное окно
    ColumnLayout {
        id: mainLayout
        anchors.fill: parent

        StackLayout {
            id: mainStack
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: 0
            Rectangle { color: "transparent" }
            ConnectAPIScreen { id: connectAPI }
            TradeScreen { id: tradePage }
        } // mainStack
    } // mainLayout

    footer: CustomStatusBar {
        id: statusWidget
        height: 40
        anchors.margins: Theme.margins
        autoHide: false
        //durationHide: 3000
        //intervalHide: 1000
    } // statusWidget

    Loader { id: windowLoader }

}
