import QtQuick
import QtQuick.Controls
import QtQuick.Window
import QtQuick.Layouts
import "components"

ApplicationWindow {
    id: window

    width: Screen.width
    height: Screen.height
    title: qsTr("info_overlay")
    color: "transparent"

    // SINGLE SOURCE OF TRUTH FOR CONTROLS
    property bool crosshairActive: false
    property bool raindropsActive: false
    property bool topbarModeActive: false

    // State properties managed by StatsTimer updates
    property string currentTime: "Time: --:--"
    property string currentDate: "Date: --/--/----"
    property string currentSession: "Session: -- h -- m"

    // 1. Rain Drops Layer
    RainOverlay {
        active: window.raindropsActive && portalShortcuts.displayMode !== 2
    }

    // 2. Crosshair Layer
    Crosshair {
        visible: window.crosshairActive && portalShortcuts.displayMode !== 2
    }

    // 3. Main Card UI Framework
    OverlayCard {
        x: window.width - width - 24
        y: 24

        // Clean layout rule: Card drops if in View-Only (1) AND TopBar is activated
        visible: portalShortcuts.displayMode === 0 ||
        (portalShortcuts.displayMode === 1 && !window.topbarModeActive)

        showInteractive: portalShortcuts.displayMode === 0

        // Push data to child sections
        timeText: window.currentTime
        dateText: window.currentDate
        sessionText: window.currentSession
    }

    // 4. Top Bar Layer
    TopBar {
        objectName: "topBarItem"

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }

        // Clean layout rule: TopBar shows strictly when View-Only (1) AND active
        visible: portalShortcuts.displayMode === 1 && window.topbarModeActive

        timeText: window.currentTime
        dateText: window.currentDate
        sessionText: window.currentSession
    }

    // 5. Time Management Processing Engine
    StatsTimer {
        onUpdated: (timeStr, dateStr, sessionStr) => {
            window.currentTime = timeStr;
            window.currentDate = dateStr;
            window.currentSession = sessionStr;
        }
    }
}
