import QtQuick

Timer {
    id: timer
    interval: 1000
    running: true
    repeat: true

    property int totalSeconds: 0

    // Custom signal to broadcast text changes
    signal updated(string timeStr, string dateStr, string sessionStr)

    onTriggered: {
        totalSeconds += 1
        var hours = Math.floor(totalSeconds / 3600)
        var minutes = Math.floor((totalSeconds % 3600) / 60)
        var formattedTime = hours + "h " + minutes + "m"

        var timeStr = "Time: " + new Date().toLocaleTimeString(Qt.locale(), Locale.ShortFormat)
        var dateStr = "Date: " + new Date().toLocaleDateString(Qt.locale(), Locale.ShortFormat)
        var sessionStr = "Session: " + formattedTime

        updated(timeStr, dateStr, sessionStr)
    }
}
