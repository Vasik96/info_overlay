import QtQuick
import QtQuick.Controls
import QtQuick.Window
import QtQuick.Layouts

// NOTE:

// Display Modes (portalShortcuts.displayMode - from C++):
// 0 = Interactive (Full UI)
// 1 = View-Only   (Stats Only)
// 2 = Hidden      (All Hidden)


ApplicationWindow {
    id: window

    width: Screen.width
    height: Screen.height
    title: qsTr("info_overlay")
    color: "transparent"

    // ==========================================
    // CROSSHAIR COMPONENT
    // ==========================================
    Item {
        id: crosshair
        objectName: "crosshairItem"
        anchors.centerIn: parent
        width: 40
        height: 40
        visible: crosshairToggle.checked && portalShortcuts.displayMode !== 2

        property int thickness: 2
        property int lineLength: 5
        property int gap: 2
        property int outline: 1

        Rectangle {
            id: leftLineOutline
            width: crosshair.lineLength + (crosshair.outline * 2)
            height: crosshair.thickness + (crosshair.outline * 2)
            color: "black"
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.horizontalCenter
            anchors.rightMargin: crosshair.gap - crosshair.outline
            Rectangle { width: crosshair.lineLength; height: crosshair.thickness; color: "red"; anchors.centerIn: parent }
        }

        Rectangle {
            id: rightLineOutline
            width: crosshair.lineLength + (crosshair.outline * 2)
            height: crosshair.thickness + (crosshair.outline * 2)
            color: "black"
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.horizontalCenter
            anchors.leftMargin: crosshair.gap - crosshair.outline
            Rectangle { width: crosshair.lineLength; height: crosshair.thickness; color: "red"; anchors.centerIn: parent }
        }

        Rectangle {
            id: topLineOutline
            width: crosshair.thickness + (crosshair.outline * 2)
            height: crosshair.lineLength + (crosshair.outline * 2)
            color: "black"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.verticalCenter
            anchors.bottomMargin: crosshair.gap - crosshair.outline
            Rectangle { width: crosshair.thickness; height: crosshair.lineLength; color: "red"; anchors.centerIn: parent }
        }

        Rectangle {
            id: bottomLineOutline
            width: crosshair.thickness + (crosshair.outline * 2)
            height: crosshair.lineLength + (crosshair.outline * 2)
            color: "black"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.verticalCenter
            anchors.topMargin: crosshair.gap - crosshair.outline
            Rectangle { width: crosshair.thickness; height: crosshair.lineLength; color: "red"; anchors.centerIn: parent }
        }
    }

    // ==========================================
    // MAIN OVERLAY WINDOW CARD
    // ==========================================
    Rectangle {
        id: overlayCard
        objectName: "overlayCard"
        clip: true

        width: 200
        height: mainLayout.implicitHeight + 32

        color: Qt.rgba(0.15, 0.16, 0.18, 0.92)
        border.color: Qt.rgba(0.31, 0.35, 0.41, 0.6)
        border.width: 1

        x: window.width - width - 24
        y: 24


        // Main Layout Panel Container
        ColumnLayout {
            id: mainLayout
            anchors.fill: parent
            anchors.margins: 16
            spacing: 12

            // ==========================================
            // SECTION 1: VIEW-ONLY CONTAINER
            // ==========================================
            ColumnLayout {
                id: viewOnlySection
                Layout.fillWidth: true
                spacing: 6

                Label {
                    text: qsTr("Info Overlay")
                    color: "#eff0f1"
                    font.pixelSize: 13
                    font.bold: true
                    Layout.alignment: Qt.AlignHCenter
                }

                Rectangle {
                    Layout.fillWidth: true
                    height: 1
                    color: Qt.rgba(0.31, 0.35, 0.41, 0.4)
                }

                Label {
                    id: timeDisplay
                    text: "Time: --:--"
                    color: "#bdc3c7"
                    font.pixelSize: 12
                    Layout.alignment: Qt.AlignHCenter
                }

                Label {
                    id: dateDisplay
                    text: "Date: --/--/----"
                    color: "#bdc3c7"
                    font.pixelSize: 12
                    Layout.alignment: Qt.AlignHCenter
                }

                Label {
                    id: sessionTimeDisplay
                    text: "Session: -- h -- m"
                    color: "#bdc3c7"
                    font.pixelSize: 12
                    Layout.alignment: Qt.AlignHCenter
                }
            }

            // ==========================================
            // SECTION 2: INTERACTIVE CONTROLS CONTAINER
            // ==========================================
            ColumnLayout {
                id: interactiveSection
                Layout.fillWidth: true
                spacing: 12

                // Directly tracks the native displayMode state safely
                visible: portalShortcuts.displayMode === 0

                Rectangle {
                    Layout.fillWidth: true
                    height: 1
                    color: Qt.rgba(0.31, 0.35, 0.41, 0.4)
                }

                CheckBox {
                    id: crosshairToggle
                    text: qsTr("Show Crosshair")
                    Layout.fillWidth: true
                    checked: false

                    contentItem: Text {
                        text: crosshairToggle.text
                        font: crosshairToggle.font
                        color: "#eff0f1"
                        leftPadding: crosshairToggle.indicator.width + crosshairToggle.spacing
                        verticalAlignment: Text.AlignVCenter
                    }
                }

                Label {
                    text: qsTr("Launch common apps:")
                    color: "#95a5a6"
                    font.pixelSize: 10
                    font.capitalization: Font.AllUppercase
                    Layout.leftMargin: 2
                }

                Button {
                    id: launchButton
                    text: qsTr("Select Application")
                    Layout.fillWidth: true
                    onClicked: appMenu.open()

                    Menu {
                        id: appMenu
                        y: parent.height
                        width: launchButton.width

                        MenuItem {
                            text: "Steam"
                            onTriggered: appLauncher.launchApp("Steam")
                        }
                        MenuItem {
                            text: "Discord"
                            onTriggered: appLauncher.launchApp("Discord")
                        }
                        MenuItem {
                            text: "Firefox"
                            onTriggered: appLauncher.launchApp("Firefox")
                        }
                    }
                }


                Rectangle {
                    Layout.fillWidth: true
                    height: 1
                    color: Qt.rgba(0.31, 0.35, 0.41, 0.4)
                }

                Button {
                    text: qsTr("Exit")
                    Layout.fillWidth: true
                    onClicked: Qt.quit()
                }

                Label {
                    text: "Switch Mode: " + portalShortcuts.shortcutLabel
                    color: "#7f8c8d"
                    font.pixelSize: 10
                    Layout.alignment: Qt.AlignHCenter
                    Layout.topMargin: 4
                }
            }
        }
    }

    // ==========================================
    // STATS UPDATER TIMER ENGINE
    // ==========================================
    Timer {
        id: timer
        interval: 1000
        running: true
        repeat: true

        property int totalSeconds: 0

        onTriggered: {
            totalSeconds += 1
            var hours = Math.floor(totalSeconds / 3600)
            var minutes = Math.floor((totalSeconds % 3600) / 60)
            var formattedTime = hours + "h " + minutes + "m"

            timeDisplay.text = "Time: " + new Date().toLocaleTimeString(Qt.locale(), Locale.ShortFormat)
            dateDisplay.text = "Date: " + new Date().toLocaleDateString(Qt.locale(), Locale.ShortFormat)
            sessionTimeDisplay.text = "Session: " + formattedTime
        }
    }
}
