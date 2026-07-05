import QtQuick
import QtQuick.Controls
import QtQuick.Layouts



Rectangle {
    id: root

    property string timeText
    property string dateText
    property string sessionText

    height: 34

    color: Qt.rgba(0.18, 0.20, 0.22, 0.85)


    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 12
        anchors.rightMargin: 12

        spacing: 16

        Label {
            text: root.timeText.replace("Time: ","")
            color: "#eff0f1"
        }

        DividerVertical {}


        Label {
            text: root.dateText.replace("Date: ","")
            color: "#eff0f1"
        }

        DividerVertical {}

        Label {
            text: root.sessionText
            color: "#eff0f1"
        }



        Item {
            Layout.fillWidth: true
        }


        Label {
            text: "Info Overlay"
            color: "#eff0f1"
            font.bold: true
        }

        Item {
            Layout.fillWidth: true
        }



        Label {
            text: "CPU: " +
            systemGlobals.cpuUsage +
            " (" + systemGlobals.cpuTemp + ")"

            color: "#eff0f1"
        }

        DividerVertical {}

        Label {
            text: "RAM: " + systemGlobals.ramUsage
            color: "#eff0f1"
        }
    }
}
