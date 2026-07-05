import QtQuick

Item {
    id: crosshair
    objectName: "crosshairItem"
    anchors.centerIn: parent
    width: 40
    height: 40

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
