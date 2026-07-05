import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ColumnLayout {
    id: viewOnlySection
    Layout.fillWidth: true
    spacing: 6

    // Expose string properties to allow Main.qml/Timer updates
    property string timeText: "Time: --:--"
    property string dateText: "Date: --/--/----"
    property string sessionText: "Session: -- h -- m"

    Label {
        text: qsTr("Info Overlay")
        color: "#eff0f1"
        font.pixelSize: 13
        font.bold: true
        Layout.alignment: Qt.AlignHCenter
    }

    Divider {}

    Label {
        text: viewOnlySection.timeText
        color: "#e2e6e9"
        font.pixelSize: 12
        Layout.alignment: Qt.AlignHCenter
    }

    Label {
        text: viewOnlySection.dateText
        color: "#e2e6e9"
        font.pixelSize: 12
        Layout.alignment: Qt.AlignHCenter
    }

    Label {
        text: viewOnlySection.sessionText
        color: "#e2e6e9"
        font.pixelSize: 12
        Layout.alignment: Qt.AlignHCenter
    }

    Label {
        text: "CPU: " + systemGlobals.cpuUsage + " (" + systemGlobals.cpuTemp + ")"
        color: "#e2e6e9"
        font.pixelSize: 12
        Layout.alignment: Qt.AlignHCenter
    }

    Label {
        text: "RAM: " + systemGlobals.ramUsage
        color: "#e2e6e9"
        font.pixelSize: 12
        Layout.alignment: Qt.AlignHCenter
    }
}
