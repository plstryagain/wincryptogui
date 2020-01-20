import QtQuick 2.0
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12

Popup {
    modal: true
    closePolicy: Popup.NoAutoClose
    parent: Overlay.overlay
    anchors.centerIn: Overlay.overlay

    RowLayout {
        anchors.fill: parent
        BusyIndicator {
            id: biWait

        }
        Label {
            id: lbWait
            Layout.fillWidth: true
            text: "Please wait..."
        }
    }
}
