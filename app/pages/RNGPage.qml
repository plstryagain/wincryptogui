import QtQuick 2.0
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12

import backend 1.0
import "qrc:/dialogs/"

Page {
    title: qsTr("RNG")
    Backend {
        id: backend
    }

    WaitDialog {
        id: dlgWait
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Label {
                text: qsTr("Select algorithm:")
            }
            ComboBox {
                id: cbRngAlgs
                Layout.fillWidth: true
                model: []
            }
        }

        RowLayout {
            Layout.fillHeight: true
            Layout.fillWidth: true

            Label {
                text: "Generate (99 kB max):"
            }
            TextField {
                id: tfBufLen
                Layout.fillWidth: true
                validator: IntValidator { bottom: 1; top: 32000 }
            }
            Label {
                text: "bytes"
            }
        }

        RowLayout {
            Layout.fillHeight: true
            Layout.fillWidth: true

            Label {
                text: "Output form:"
            }
            RadioButton {
                id: rbFormHex
                text: "HEX"
                checked: true
            }
            RadioButton {
                id: rbFormBase64
                text: "Base64"
            }
            Item {
                Layout.fillWidth: true
            }
        }

        Button {
            id: btnGenerateRandom
            Layout.alignment: Qt.AlignCenter
            text: "Generate random bytes"

            onClicked: {
                dlgWait.open();
                const alg_id = cbRngAlgs.currentText;
                const size = tfBufLen.text;
                let out_form = '';
                if (rbFormHex.checked) {
                    out_form = "HEX";
                } else {
                    out_form = "Base64";
                }

                backend.generateRandom(alg_id, size, out_form);
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
                wrapMode: Text.WrapAnywhere
                selectByMouse: true
            }
        }
    }

    Connections {
        target: backend

        onNotifyRngAlgsEnumComplete: {
           dlgWait.close();
            if (err === 0) {
                cbRngAlgs.model = alg_id_list;
            } else {
                txtResult.append("Operation failed with error: " + err + "\n");
            }
        }

        onNotifyRandomBytesGenerated: {
            if (err === 0) {
                txtResult.append("\nGenerated:");
                txtResult.append(bytes);
            } else {
                txtResult.append("Operation failed with error: " + err + "\n");
            }
            dlgWait.close();
        }
    }

    Component.onCompleted: {
        tfBufLen.text = "32";
        dlgWait.open();
        backend.enumRngAlgorithms();
    }
}
