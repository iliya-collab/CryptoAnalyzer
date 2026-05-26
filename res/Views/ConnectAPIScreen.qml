import QtQuick 2.15
import QtQuick.Layouts 1.15

import Engine 1.0
import Components 1.0
import Theme 1.0

Rectangle {
    id: root
    color: Theme.windowColor

    ColumnLayout {
        anchors.margins: Theme.margins
        anchors.horizontalCenter: parent.horizontalCenter

        ColumnLayout {
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

        ColumnLayout {
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
                    regularExpression: /^[a-zA-Z0-9]{32}$/
                }
            }
        }

        ColumnLayout {
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
                checked: true
            }
        }

        CustomButton {
            text: "Connect"
            Layout.margins: Theme.margins
            Layout.alignment: Qt.AlignRight | Qt.AlignBottom
            enabled: txtfAPIKey.acceptableInput && txtfSecretAPI.acceptableInput
            onClicked: {
                Engine.setAPI(txtfAPIKey.text, txtfSecretAPI.text, chbTNetwork.checked)
                Engine.checkAPI()
            }
        }

    } // ColumnLayout
}
