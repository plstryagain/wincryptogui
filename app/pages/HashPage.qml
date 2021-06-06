import QtQuick 2.0
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.3
import Qt.labs.platform 1.1

import backend 1.0
import "qrc:/dialogs/"

Page {
    title: qsTr("Hash")
    Backend {
        id: backend
    }

    WaitDialog {
        id: dlgWait
    }

    FileDialog {
        id: dlgSelectFile

        onAccepted: {
            tfChooseFile.text = file.toString();
        }
    }

    FileDialog {
        id: dlgChooseFirstFile

        onAccepted: {
            tfChooseFirstFile.text = file.toString();
        }
    }

    FileDialog {
        id: dlgChooseSecondFile

        onAccepted: {
            tfChooseSecondFile.text = file.toString();
        }
    }

    FolderDialog {
        id: dlgSelectFolder

        onAccepted: {
            tfChooseFile.text = folder.toString();
        }
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

                        ToolTip {
                            id: ttChooseFile
                            timeout: 3000
                            visible: false
                        }
                    }
                    Button {
                        id: btnChooseFile
                        text: "..."

                        onClicked: {
                            if (rbOneFile.checked) {
                                dlgSelectFile.open();
                            } else {
                                dlgSelectFolder.open();
                            }
                        }
                    }
                }
            }

        }
        GroupBox {
            id: gbCompareFiles
            Layout.fillWidth: true
            visible: false

            ColumnLayout {
                anchors.fill: parent
                RowLayout {
                    Layout.fillWidth: true
                    Label {
                        id: lbChooseFirstFile
                        text: "Choose first file"
                    }
                    TextField {
                        id: tfChooseFirstFile
                        Layout.fillWidth: true

                        ToolTip {
                            id: ttChooseFIrstFile
                            timeout: 3000
                            visible: false
                            text: "Choose first file!"
                        }
                    }
                    Button {
                        text: "..."

                        onClicked: {
                            dlgChooseFirstFile.open();
                        }
                    }
                }
                RowLayout {
                    Layout.fillWidth: true
                    Label {
                        id: lbChooseSecondFile
                        text: "Choose second file"
                    }
                    TextField {
                        id: tfChooseSecondFile
                        Layout.fillWidth: true

                        ToolTip {
                            id: ttChooseSecondFile
                            timeout: 3000
                            visible: false
                            text: "Choose second file!"
                        }
                    }
                    Button {
                        text: "..."

                        onClicked: {
                            dlgChooseSecondFile.open();
                        }
                    }
                }
            }
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
                    backend.calculateHash(path, is_dir, alg_id);
                } else {
                    let first_file = tfChooseFirstFile.text;
                    let second_file = tfChooseSecondFile.text;
                    if (first_file.length === 0) {
                        ttChooseFIrstFile.visible = true;
                        return;
                    }
                    if (second_file.length === 0) {
                        ttChooseSecondFile.visible = true;
                        return;
                    }
                    let alg_id = cbHashAlgs.currentText;
                    dlgWait.open();
                    backend.compareFiles(first_file, second_file, alg_id);
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
                selectByMouse: true
            }
        }
    }

    Connections {
        target: backend

        onNotifyOperationFinished: {
            dlgWait.close();
        }

        onNotifyHashAlgsEnumComplete: {
            dlgWait.close();
            if (err === 0) {
                cbHashAlgs.model = alg_id_list;
            } else {
                console.log("error: " + err)
                txtResult.append("Operation failed with error: " + err);
            }
        }

        onNotifyOneHashCalculated: {
            if (err === 0) {
                let str = "For file: " + file_name + "\nAlgorithm: " + alg_id + "\nHash: " + hash + "\n";
                txtResult.append(str);
            } else {
                let str = "For file: " + file_name + "\nAlgorithm: " + alg_id + "\nError: " + err + "\n";
                txtResult.append(str);
            }
        }

        onNotifyFilesCompared: {
            if (err == 0) {
                if (is_equal) {
                    txtResult.append("Files are identical!");
                    txtResult.append("Hash value: " + first_hash + "\n");
                } else {
                    txtResult.append("Files are not identical!");
                    txtResult.append("First file hash value: " + first_hash);
                    txtResult.append("Second file hash value: " + second_hash + "\n");
                }
            } else {
                txtResult.append("Operation failed with error: " + err);
            }
        }
    }

    Component.onCompleted: {
        dlgWait.open();
        backend.enumHashAlgorithms();
    }
}
