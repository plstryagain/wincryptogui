import QtQuick 2.0
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.3
import Qt.labs.platform 1.1

import backend 1.0
import "qrc:/dialogs/"

Page {
    title: qsTr("Cipher")

    Backend {
        id: backend
    }

    WaitDialog {
        id: dlgWait
    }

    FileDialog {
        id: dlgSelectFile

        property string mode: "encrypt"

        onAccepted: {
            if (mode === "encrypt") {
                tfFileToEncrypt.text = file.toString();
            } else {

            }
        }
    }

    FolderDialog {
        id: dlgPathToSave

        property string mode: "encrypt"

        onAccepted: {
            if (mode === "encrypt") {
                tfPathToSaveCipher.text = folder.toString();
            } else {

            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 10
        anchors.margins: 10
        RowLayout {
            Layout.fillWidth: true
            RadioButton {
                id: rbEncrypt
                text: qsTr("Encrypt")
                checked: true

                onCheckedChanged: {
                    if (checked) {
                        gbEncrypt.visible = true;
                    } else {
                        gbEncrypt.visible = false;
                    }
                }
            }
            RadioButton {
                id: rbDecrypt
                text: qsTr("Decrypt")

                onCheckedChanged: {
                    if (checked) {
                        gbDecrypt.visible = true;
                    } else {
                        gbDecrypt.visible = false;
                    }
                }
            }
        }
        RowLayout {
            Layout.fillWidth: true
            Label {
                id: lbSelectAlg
                text: qsTr("Select algorithm")
            }
            ComboBox {
                id: cbAlgs
                Layout.fillWidth: true
                model: []
            }
        }
        GroupBox {
            id: gbEncrypt
            Layout.fillWidth: true
            GridLayout {
                anchors.fill: parent
                columns: 3
                Label {
                    text: qsTr("Enter password")
                }
                TextField {
                    id: tfPass
                    Layout.fillWidth: true
                    passwordCharacter: "*"
                    passwordMaskDelay: 3000
                    echoMode: TextInput.Password

                    property bool secret: true

                    ToolTip {
                        id: ttPass
                        timeout: 3000
                        visible: false
                        text: qsTr("Enter password!")
                    }
                }
                Button {
                    id: btnShowPass
                    text: tfPass.secret ? qsTr("Show") : qsTr("Hide")

                    onClicked: {
                        if (tfPass.secret) {
                            tfPass.echoMode = TextInput.Normal;
                            tfPass.secret = false;
                        } else {
                            tfPass.echoMode = TextInput.Password;
                            tfPass.secret = true;
                        }
                    }
                }

                Label {
                    text: qsTr("Select file")
                }
                TextField {
                    id: tfFileToEncrypt
                    Layout.fillWidth: true

                    ToolTip {
                        id: ttFileToEncrypt
                        timeout: 3000
                        visible: false
                        text: qsTr("Select file to Encrypt!")
                    }
                }
                Button {
                    text: qsTr("...")

                    onClicked: {
                        dlgSelectFile.mode = "encrypt";
                        dlgSelectFile.open();
                    }
                }

                Label {
                    text: qsTr("Save to")
                }
                TextField {
                    id: tfPathToSaveCipher
                    Layout.fillWidth: true

                    ToolTip {
                        id: ttPathToSaveCipher
                        timeout: 3000
                        visible: false
                        text: qsTr("Select where to save ciphertext!")
                    }
                }
                Button {
                    text: qsTr("...")

                    onClicked: {
                        dlgPathToSave.mode = "encrypt";
                        dlgPathToSave.open();
                    }
                }
            }
        }
        GroupBox {
            id: gbDecrypt
            Layout.fillWidth: true
            visible: false
            GridLayout {
                anchors.fill: parent
            }
        }
        Button {
            id: btnEncryptDecrypt
            text: rbEncrypt.checked ? qsTr("Encrypt") : qsTr("Decrypt")
            Layout.alignment: Qt.AlignCenter

            onClicked: {
                if (rbEncrypt.checked) {
                    if (tfPass.text === '') {
                        ttPass.visible = true;
                        return;
                    }
                    if (tfFileToEncrypt.text === '') {
                        ttFileToEncrypt.visible = true;
                        return;
                    }
                    if (tfPathToSaveCipher.text === '') {
                        tfPathToSaveCipher.visible = true;
                        return;
                    }

                    dlgWait.open();
                    backend.encrypt(cbAlgs.currentText, tfPass.text, tfFileToEncrypt.text, tfPathToSaveCipher.text);
                }
            }
        }
        ScrollView {
            id: view
            Layout.fillHeight: true
            Layout.fillWidth: true
            TextArea {
                id: txtResult
                implicitWidth: 100
                readOnly: true
                wrapMode: TextArea.WrapAnywhere
                selectByMouse: true
            }
        }
    }

    Connections {
        target: backend

        onNotifyCipherAlgsEnumCompleted: {
            dlgWait.close();
            if (err === 0) {
                cbAlgs.model = alg_id_list;
            } else {
                console.log("error: " + err)
                txtResult.append("Operation failed with error: " + err);
            }
        }

        onNotifyEncrypted: {
            dlgWait.close();
            if (err === 0) {
                txtResult.append(cipher_text);
            } else {
                console.log("error: " + err)
                txtResult.append("Operation failed with error: " + err);
            }
        }
    }

    Component.onCompleted: {
        dlgWait.open();
        backend.enumCipherAlgorithms();
    }
}
