import QtQuick 2.0
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12

import backend 1.0
import "qrc:/dialogs/"

Page {

    Backend {
        id: backend
    }

    WaitDialog {
        id: dlgWait
    }

    ColumnLayout {
        id: mainLayout
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10
        RowLayout{
            Layout.fillWidth: true
            RadioButton {
                id: rbCalculateHash
                text: "Calculate hash"
                checked: true

                onCheckedChanged: {
                    if (checked) {
                        gbCalculateHash.visible = true;
                    } else {
                        gbCalculateHash.visible = false;
                    }
                }
            }
            RadioButton {
                id: rbCompareFiles
                text: "Compare files"

                onCheckedChanged: {
                    if (checked) {
                        gbCompareFiles.visible = true;
                    } else {
                        gbCompareFiles.visible = false;
                    }
                }
            }
        }
        RowLayout {
            Layout.fillWidth: true
            Label {
                text: "Select algorithm:"
            }
            ComboBox {
                id: cbHashAlgs
                Layout.fillWidth: true
                model: []
            }
        }

        GroupBox {
            id: gbCalculateHash
            Layout.fillWidth: true

            ColumnLayout {
                anchors.fill: parent
                RowLayout {
                    Layout.fillWidth: true
                    RadioButton {
                        id: rbOneFile
                        text: "One file"
                        checked: true
                    }
                    RadioButton {
                        id: rbDirectory
                        text: "All files in directory"
                    }
                }
                RowLayout {
                    Layout.fillWidth: true
                    Label {
                        id: lbChooseFile
                        text: rbOneFile.checked ? "Choose file" : "Choose folder"
                    }
                    TextField {
                        Layout.fillWidth: true
                        id: tfChooseFile
                        text: "D:\\ad.png"

                        ToolTip {
                            id: ttChooseFile
                            timeout: 3000
                            visible: false
                        }
                    }
                    Button {
                        id: btnChooseFile
                        text: "..."
                    }
                }
            }

        }
        GroupBox {
            id: gbCompareFiles
            Layout.fillWidth: true
            visible: false
        }
        Button {
            id: btnHash
            Layout.alignment: Qt.AlignCenter
            text: rbCalculateHash.checked ? "Calculate" : "Compare"

            onClicked: {
                if (rbCalculateHash.checked) {
                    let is_dir = rbDirectory.checked;
                    let path = tfChooseFile.text;
                    if (path.length === 0) {
                        ttChooseFile.text = is_dir ? "Please select folder!" : "Please select file";
                        ttChooseFile.visible = true;
                        return;
                    }
                    let alg_id = cbHashAlgs.currentText;
                    dlgWait.open();
                    backend.calculateHash(tfChooseFile.text, is_dir, alg_id);
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
            }
        }
    }

    Connections {
        target: backend

        onNotifyHashAlsEnumComplete: {
            dlgWait.close();
            if (err === 0) {
                cbHashAlgs.model = alg_id_list;
            } else {
                console.log("error: " + err)
            }
        }

        onNotifyOneHashCalculated: {
            dlgWait.close();
            if (err === 0) {
                let str = "For file: " + file_name + "\nAlgorithm: " + alg_id + "\nHash: " + hash + "\n";
                txtResult.append(str);
            } else {
                let str = "For file: " + file_name + "\nAlgorithm: " + alg_id + "\nError: " + err + "\n";
                txtResult.append(str);
            }
        }
    }

    Component.onCompleted: {
        dlgWait.open();
        backend.enumHashAlgorithms();
    }
}
