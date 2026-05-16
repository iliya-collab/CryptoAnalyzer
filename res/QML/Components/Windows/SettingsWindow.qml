import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Window {
    id: settingsWindow

    width: 400
    height: 300

    // Фиксируем размер
    minimumWidth: width
    maximumWidth: width
    minimumHeight: height
    maximumHeight: height

    // Убираем кнопку "Развернуть" из заголовка
    flags: Qt.Window | Qt.WindowCloseButtonHint | Qt.WindowMinimizeButtonHint

    visible: true
    title: "Settings"
    color: theme.windowColor

    ColumnLayout {
        anchors.fill: parent

        // Панель вкладок
        TabBar {
            id: tabBar
            currentIndex: 0 // Вкладка по умолчанию
            Layout.fillWidth: true
            background: Rectangle { color: theme.toolBarColor }

            TabButton {
                id: btnSelect
                text: "Select key"

                background: Rectangle {
                    implicitWidth: 100
                    implicitHeight: 40
                    color: theme.tabsColor
                    border.color: theme.borderColor
                    border.width: 2
                }

                contentItem: Text {
                    text: btnSelect.text
                    font: btnSelect.font
                    color: theme.textColor
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
            TabButton {
                id: btnCreate
                text: "Create key"

                background: Rectangle {
                    implicitWidth: 100
                    implicitHeight: 40
                    color: theme.tabsColor
                    border.color: theme.borderColor
                    border.width: 2
                }

                contentItem: Text {
                    text: btnCreate.text
                    font: btnCreate.font
                    color: theme.textColor
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }

        // Содержимое вкладок
        StackLayout {
            id: container
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: tabBar.currentIndex

            // Select key
            Rectangle {
                color: theme.windowColor
                Label {
                    text: "Содержимое 1";
                    anchors.centerIn: parent
                }
            }

            // Create key
            Rectangle {
                color: theme.windowColor

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 10

                    RowLayout {

                        Label {
                            text: "Name:"
                            font.pixelSize: 12
                            color: theme.textColor
                            horizontalAlignment: Text.AlignHCenter
                        }

                        TextField { id: txtfName }

                    }

                    RowLayout {

                        Label {
                            text: "Type network:"
                            font.pixelSize: 12
                            color: theme.textColor
                            horizontalAlignment: Text.AlignHCenter
                        }

                        CheckBox {
                            id: chbTNetwork
                            text: "Testnet"
                            checked: true
                        }

                    }

                    RowLayout {

                        Label {
                            text: "API key:"
                            font.pixelSize: 12
                            color: theme.textColor
                            horizontalAlignment: Text.AlignHCenter
                        }

                        TextField {
                            id: txtfAPIKey
                            // Ограничиваем максимальную длину
                            maximumLength: 20

                            // Валидатор: разрешает только A-Z, a-z и 0-9
                            validator: RegularExpressionValidator {
                                regularExpression: /^[a-zA-Z0-9]{18,20}$/
                            }

                        }

                    }

                    RowLayout {

                        Label {
                            text: "Secret API key:"
                            font.pixelSize: 12
                            color: theme.textColor
                            horizontalAlignment: Text.AlignHCenter
                        }

                        TextField {
                            id: txtfSecretAPI
                            // Ограничиваем максимальную длину
                            maximumLength: 32

                            // Валидатор: разрешает только A-Z, a-z и 0-9
                            validator: RegularExpressionValidator {
                                regularExpression: /^[a-zA-Z0-9]{32}$/
                            }

                        }

                    }

                    Item { Layout.fillHeight: true }

                    Button {
                        Layout.alignment: Qt.AlignRight | Qt.AlignBottom
                        text: "Save"

                        enabled: (txtfName.text.length > 0) && txtfAPIKey.acceptableInput && txtfSecretAPI.acceptableInput

                        onClicked: appEngine.saveAPI(txtfName.text, txtfAPIKey.text, txtfSecretAPI.text, chbTNetwork.checked)
                    }

                } // ColumnLayout
            } // Create key
        }
    }


} // settingsWindow
