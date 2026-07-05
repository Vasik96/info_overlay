import QtQuick
import QtQuick.Window

Item {
    id: rainRoot
    objectName: "rainRootItem"
    anchors.fill: parent
    z: -1

    property bool active: false
    visible: active

    property real shaderTime: 0
    NumberAnimation on shaderTime {
        from: 0
        to: 10000
        duration: 1000000
        loops: Animation.Infinite
        running: rainRoot.visible
    }

    ShaderEffect {
        anchors.fill: parent
        blending: true
        property real iTime: rainRoot.shaderTime
        property size iResolution: Qt.size(Screen.width, Screen.height)
        fragmentShader: "qrc:/info_overlay/src/shaders/raindrops.frag.qsb"
    }
}
