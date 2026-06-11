import QtQuick
import QtQuick.Controls
import com.overlay.file 1.0

ApplicationWindow {
    id: window

    FileHandler {
        id: fileHandler
    }

    width: 1920
    height: 1080
    title: qsTr("info_overlay")
    color: "transparent"


    Rectangle {
        id: overlayCard
        objectName: "overlayCard"
        clip: true
        width: 280
        height: 270
        radius: 6

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



            Text {
                text: "Notes"
                color: "lightgray"
                font.pixelSize: 12
                anchors.left: parent.left
            }


            ScrollView {
                width: 250
                height: 130
                clip: true

                // This policy ensures it only shows when text exceeds the 90px height
                ScrollBar.vertical.policy: ScrollBar.AlwaysOn

                TextArea {
                    id: notesArea
                    placeholderText: "Type your notes here..."
                    color: "white"

                    // Ensure text wraps so it doesn't just create a super-long single line
                    wrapMode: TextArea.Wrap

                    background: Rectangle {
                        color: "transparent";
                        border.color: "gray"
                    }

                    onActiveFocusChanged: {
                        if (activeFocus) {
                            fileHandler.setKeyboardMode(true)
                        } else {
                            fileHandler.setKeyboardMode(false)
                        }
                    }

                    onTextChanged: {
                        fileHandler.saveText(notesArea.text)
                    }
                }
            }





            Button {
                text: "Exit"
                anchors.horizontalCenter: parent.horizontalCenter
                onClicked: {
                    Qt.quit()
                }
            }

            Component.onCompleted: {
                notesArea.text = fileHandler.loadText()
            }
        }
    }
}
