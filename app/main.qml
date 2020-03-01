import QtQuick 2.12
import QtQuick.Controls 2.5

ApplicationWindow {
    id: window
    visible: true
    width: 640
    height: 480
    title: qsTr("Wincryptogui")

    header: ToolBar {
        contentHeight: toolButton.implicitHeight

        ToolButton {
            id: toolButton
            text: stackView.depth > 1 ? "\u25C0" : "\u2630"
            font.pixelSize: Qt.application.font.pixelSize * 1.6
            onClicked: {
                if (stackView.depth > 1) {
                    stackView.pop()
                } else {
                    drawer.open()
                }
            }
        }

        Label {
            text: stackView.currentItem.title
            anchors.centerIn: parent
        }
    }

    Drawer {
        id: drawer
        width: window.width * 0.66
        height: window.height

        Column {
            anchors.fill: parent

            ItemDelegate {
                text: qsTr("Hash")
                width: parent.width
                onClicked: {
                    stackView.push("qrc:/pages/HashPage.qml");
                    drawer.close();
                }
            }
            ItemDelegate {
                text: qsTr("Random number generator")
                width: parent.width
                onClicked: {
                    stackView.push("qrc:/pages/RNGPage.qml");
                    drawer.close();
                }
            }
            ItemDelegate {
                text: qsTr("Cipher")
                width: parent.width
                onClicked: {
                    stackView.push("qrc:/pages/CipherPage.qml");
                    drawer.close();
                }
            }
        }
    }

    StackView {
        id: stackView
        initialItem: "qrc:/pages/HomePage.qml"
        anchors.fill: parent
    }
}
