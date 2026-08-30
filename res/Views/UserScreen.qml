import QtQuick 2.15
import QtQuick.Layouts 1.15
import Application.Core 1.0
import Components.Custom 1.0
import Theme 1.0

Rectangle {
    id: root
    color: Theme.windowColor

    property Api apiData: AppCore.marketState.api

    ColumnLayout {
        anchors.fill: parent

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true

            ColumnLayout {
                Layout.alignment: Qt.AlignTop
                Layout.fillWidth: true
                Layout.fillHeight: true

                RowLayout {
                    CustomLabel {
                        text: "API key:"
                        Layout.margins: Theme.margins
                        horizontalAlignment: Text.AlignHCenter
                        font.pixelSize: Theme.fontSizeSmall
                    }
                    CustomTextField {
                        id: txtfAPIKey
                        Layout.margins: Theme.margins
                        text: apiData.apiKey
                        validator: RegularExpressionValidator {
                            regularExpression: /^[a-zA-Z0-9]{18,20}$/
                        }
                    }
                }

                RowLayout {
                    CustomLabel {
                        text: "Secret API key:"
                        Layout.margins: Theme.margins
                        horizontalAlignment: Text.AlignHCenter
                        font.pixelSize: Theme.fontSizeSmall
                    }
                    CustomTextField {
                        id: txtfSecretAPI
                        Layout.margins: Theme.margins
                        text: apiData.secretKey
                        validator: RegularExpressionValidator {
                            regularExpression: /^[a-zA-Z0-9]{32,36}$/
                        }
                    }
                }

                RowLayout {
                    CustomLabel {
                        text: "Type network:"
                        horizontalAlignment: Text.AlignHCenter
                        Layout.margins: Theme.margins
                        font.pixelSize: Theme.fontSizeSmall
                    }
                    CustomCheckBox {
                        id: chbTNetwork
                        Layout.margins: Theme.margins
                        text: "Testnet"
                        checked: apiData.isTestnet
                    }
                }
            } // Column

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.alignment: Qt.AlignRight | Qt.AlignTop
                Layout.margins: Theme.margins
                color: "transparent"
                border.width: Theme.borderWidth
                border.color: Theme.borderColor

                ListView {
                    id: viewAssets
                    anchors.fill: parent
                    anchors.margins: Theme.margins
                    model: AppCore.marketState.assets
                    clip: true
                    reuseItems: true

                    delegate: Row {
                        spacing: Theme.spacing
                        width: viewAssets.width

                        CustomLabel {
                            text: model.name
                            width: (parent.width - 2 * parent.spacing) / 3
                            horizontalAlignment: Text.AlignHCenter
                        }
                        CustomLabel {
                            text: model.amount
                            width: (parent.width - 2 * parent.spacing) / 3
                            horizontalAlignment: Text.AlignHCenter
                        }
                        CustomLabel {
                            text: model.value
                            width: (parent.width - 2 * parent.spacing) / 3
                            horizontalAlignment: Text.AlignHCenter
                        }
                    }
                }
            }

        }

        CustomButton {
            text: "Connect"
            Layout.margins: Theme.margins
            Layout.alignment: Qt.AlignRight | Qt.AlignBottom
            enabled: txtfAPIKey.acceptableInput && txtfSecretAPI.acceptableInput
            onClicked: {
                AppCore.marketService.loadAccountBalance
            }
        }
    } // ColumnLayout
}
