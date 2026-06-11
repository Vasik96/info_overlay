import QtQuick
import QtQuick.Controls
import QtWebEngine

ApplicationWindow {
    id: window



    width: 1920
    height: 1080
    title: qsTr("info_overlay")
    color: "transparent"

    Shortcut {
        sequences: ["Ctrl+F"]
        onActivated: {
            findBar.visible = !findBar.visible

            if (findBar.visible) {
                findInput.forceActiveFocus()
            } else {
                // When closing, clear text and clear highlights
                findInput.text = ""
                webView.findText("")
                webView.forceActiveFocus()
            }
        }
    }


    Rectangle {
        id: overlayCard
        objectName: "overlayCard"
        clip: true
        width: 950
        height: 700
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


            Row {
                id: findBar
                visible: false // Hidden by default
                spacing: 5

                TextField {
                    id: findInput
                    placeholderText: "Search..."

                    // Auto-search and highlight everything as you type
                    onTextChanged: {
                        if (text.length > 0) {
                            webView.findText(text)
                        } else {
                            webView.findText("") // Clear highlights if empty
                        }
                    }

                    // Navigation on Enter
                    Keys.onPressed: {
                        if (event.key === Qt.Key_Enter || event.key === Qt.Key_Return) {
                            if (event.modifiers & Qt.ShiftModifier) {
                                // Find Backward
                                webView.findText(text, WebEngineView.FindBackward)
                            } else {
                                // Find Forward
                                webView.findText(text)
                            }
                            event.accepted = true
                        }
                    }
                }

                Button {
                    text: "X"
                    onClicked: {
                        findBar.visible = false
                        webView.findText("") // Clears the search highlights
                    }
                }
            }


            Row {
                id: addressBar
                spacing: 5
                width: 660 // Match your WebView width

                TextField {
                    id: urlInput
                    width: 580
                    placeholderText: "Enter URL (e.g., https://google.com)"
                    text: webView.url // Initialize with current URL

                    // Navigate when pressing Enter
                    onAccepted: {
                        // Ensure the URL has a protocol
                        let url = text
                        if (!url.startsWith("http://") && !url.startsWith("https://")) {
                            url = "https://" + url
                        }
                        webView.url = url
                        webView.forceActiveFocus()
                    }
                }

                Button {
                    text: "Go"
                    onClicked: {
                        webView.url = urlInput.text
                        webView.forceActiveFocus()
                    }
                }
            }

            WebEngineView {
                id: webView
                width: 660
                height: 400
                url: "https://www.google.com"


                // 1. Force the view to accept focus
                focus: true

                onUrlChanged: {
                    urlInput.text = webView.url.toString()
                }

                // 2. Ensure it grabs focus when clicked
                MouseArea {
                    anchors.fill: parent
                    onPressed: {
                        webView.forceActiveFocus()
                        mouse.accepted = false // Pass the click through to the webview
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


        }
    }
}
