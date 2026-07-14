import QtQuick 2.15
import QtQuick.Layouts 1.15
import MainApplication 1.0
import Components.Custom 1.0
import Theme 1.0

Rectangle {
    id: root
    color: Theme.windowColor

    property api apiData: AppCore.api

    ColumnLayout {
        anchors.fill: parent

        ColumnLayout {
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

        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: parent.height / 6
            CustomButton {
                text: "Connect"
                Layout.margins: Theme.margins
                Layout.alignment: Qt.AlignRight | Qt.AlignBottom
                enabled: txtfAPIKey.acceptableInput && txtfSecretAPI.acceptableInput
                onClicked: {
                    AppCore.setAPI(txtfAPIKey.text, txtfSecretAPI.text, chbTNetwork.checked)
                    AppCore.checkAPI()
                }
            }
            CustomButton {
                text: "Save"
                Layout.margins: Theme.margins
                Layout.alignment: Qt.AlignRight | Qt.AlignBottom
                enabled: txtfAPIKey.acceptableInput && txtfSecretAPI.acceptableInput
                onClicked: {
                    AppCore.saveAPI(txtfAPIKey.text, txtfSecretAPI.text, chbTNetwork.checked)
                }
            }
        } // Row
    } // ColumnLayout
}
