import QtQuick
import QtQuick.Layouts

Rectangle {
    id: overlayCard
    objectName: "overlayCard"
    clip: true

    width: 200
    height: mainLayout.implicitHeight + 32

    color: Qt.rgba(0.125, 0.137, 0.149, 0.9)
    border.color: Qt.rgba(0.31, 0.35, 0.41, 0.6)
    border.width: 1

    property alias timeText: viewSection.timeText
    property alias dateText: viewSection.dateText
    property alias sessionText: viewSection.sessionText

    property bool showInteractive: true

    ColumnLayout {
        id: mainLayout
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        ViewOnlySection {
            id: viewSection
        }

        InteractiveSection {
            id: interactiveSection
            visible: overlayCard.showInteractive
        }
    }
}
