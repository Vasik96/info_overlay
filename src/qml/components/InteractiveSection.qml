import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ColumnLayout {
    id: interactiveSection
    Layout.fillWidth: true
    spacing: 12

    CheckBox {
        id: crosshairToggle
        text: qsTr("Show Crosshair")
        Layout.fillWidth: true
        checked: window.crosshairActive
        onCheckedChanged: window.crosshairActive = checked

        contentItem: Text {
            text: crosshairToggle.text
            font: crosshairToggle.font
            color: "#eff0f1"
            leftPadding: crosshairToggle.indicator.width + crosshairToggle.spacing
            verticalAlignment: Text.AlignVCenter
        }
    }

    CheckBox {
        id: raindropsToggle
        text: qsTr("Show Raindrops")
        Layout.fillWidth: true
        checked: window.raindropsActive
        onCheckedChanged: window.raindropsActive = checked

        contentItem: Text {
            text: raindropsToggle.text
            font: raindropsToggle.font
            color: "#eff0f1"
            leftPadding: raindropsToggle.indicator.width + raindropsToggle.spacing
            verticalAlignment: Text.AlignVCenter
        }
    }

    CheckBox {
        id: topbarModeToggle
        text: qsTr("Top bar mode")
        Layout.fillWidth: true
        checked: window.topbarModeActive
        onCheckedChanged: window.topbarModeActive = checked

        contentItem: Text {
            text: topbarModeToggle.text
            font: topbarModeToggle.font
            color: "#eff0f1"
            leftPadding: topbarModeToggle.indicator.width + topbarModeToggle.spacing
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

            MenuItem { text: "Steam"; onTriggered: appLauncher.launchApp("Steam") }
            MenuItem { text: "Discord"; onTriggered: appLauncher.launchApp("Discord") }
            MenuItem { text: "Firefox"; onTriggered: appLauncher.launchApp("Firefox") }
        }
    }

    Divider {}

    Button {
        text: qsTr("Exit")
        Layout.fillWidth: true
        onClicked: Qt.quit()
    }

    Label {
        text: "Switch Mode: " + portalShortcuts.shortcutLabel
        color: "#bdc3c7"
        font.pixelSize: 10
        Layout.alignment: Qt.AlignHCenter
        Layout.topMargin: 4
    }
}
