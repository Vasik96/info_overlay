import QtQuick
import QtQuick.Controls
import QtWebEngine

ApplicationWindow {
    id: window



    width: 1920
    height: 1080
    title: qsTr("info_overlay")
    color: "transparent"



    Rectangle {
        id: overlayCard
        objectName: "overlayCard"
        clip: true
        width: 180
        height: 200

        color: Qt.rgba(0.13, 0.13, 0.13, 0.85)
        border.color: Qt.rgba(1, 1, 1, 0.15)
        border.width: 1

        x: window.width - width - 30
        y: 30

        Column {
            anchors.centerIn: parent
            spacing: 12

            Text {
                text: "Info Overlay"
                color: "white"
                font.pixelSize: 14
                font.bold: true
                anchors.horizontalCenter: parent.horizontalCenter
            }



            Button {
                text: "Exit"
                anchors.horizontalCenter: parent.horizontalCenter
                onClicked: {
                    Qt.quit()
                }
            }


            Text {
                text: "Toggle menu: " + portalShortcuts.shortcutLabel
                color: "white"
                font.pixelSize: 12
            }


        }
    }
}
