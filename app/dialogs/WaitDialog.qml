import QtQuick 2.9
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3

Popup{
    height: 80
    parent: Overlay.overlay
    x: Math.round((parent.width - width) / 2)
    y: Math.round((parent.height - height) / 2)
    visible: false
    modal: true
    closePolicy: Popup.NoAutoClose

    property alias statusMsg: lbWait.text

    Row{
        anchors.fill: parent
        spacing: 10

        BusyIndicator{
            id: biWait
            anchors.verticalCenter: parent.verticalCenter
        }
        Label{
            id: lbWait
            anchors.verticalCenter: parent.verticalCenter
            text: qsTr("Please wait...")
        }
    }
}
