import QtQuick 2.0
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12

Page {
    title: "Home"
    ColumnLayout {
        anchors.fill: parent
        Label {
            Layout.alignment: Qt.AlignCenter
            text: "Windows Cryptography Demonstration"
            font.pointSize: 18
        }
        Label {
            Layout.alignment: Qt.AlignCenter
            text: "(using Windows CNG)"
        }
        Item {
            Layout.fillHeight: true
        }
        Label {
            Layout.alignment: Qt.AlignCenter
            text: "Version: 0.0.1"
        }
    }
}
