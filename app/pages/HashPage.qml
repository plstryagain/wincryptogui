import QtQuick 2.0
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12

import backend 1.0

Page {

    Backend {
        id: backend
    }

    ColumnLayout {
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
                        text: "Choose file"
                    }
                    TextField {
                        Layout.fillWidth: true
                        id: tfChooseFile
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
        }

        TextArea {
            id: txtResult
            Layout.fillHeight: true
            Layout.fillWidth: true
            readOnly: true
        }
    }

    Connections {
        target: backend

        onNotifyHashAlsEnumComplete: {
            console.log(err)
            console.log(alg_id_list)
            cbHashAlgs.model = alg_id_list
        }
    }

    Component.onCompleted: {
        backend.enumHashAlgorithms();
    }
}
