import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Theme 1.0
import Components.Custom 1.0
import Application.Core 1.0

Window {
    id: root
    width: 600
    height: 400
    visible: true
    title: "Create API"
    color: Theme.windowColor

    signal newAPICreated(string name, API api)

    ColumnLayout {
        anchors.fill: parent

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true

            RowLayout {
                CustomLabel {
                    id: txtNameAPI
                    text: "Name:"
                    Layout.margins: Theme.margins
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: Theme.fontSizeSmall
                }
            }

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
                }
            }
        } // Column

        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: parent.height / 6
            CustomButton {
                text: "Add"
                Layout.margins: Theme.margins
                Layout.alignment: Qt.AlignRight | Qt.AlignBottom
                enabled: txtfAPIKey.acceptableInput && txtfSecretAPI.acceptableInput
                onClicked: {
                    newAPICreated(txtNameAPI.text, ({   apiKey: txtfAPIKey.text,
                                                        secretKey: txtfSecretAPI.text,
                                                        isTestnet: chbTNetwork.enabled
                                                    }))
                }
            }
        } // Row
    } // ColumnLayout


} // settingsWindow
