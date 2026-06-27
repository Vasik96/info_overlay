#include <QDir>
#include <QDebug>
#include <QFileInfo>
#include <QTextStream>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickWindow>
#include <QStandardPaths>

#include "OverlayMaskHandler.h"
#include "PortalShortcuts.h"

// ---------------------------------------------------------------------------
// Writes an XDG .desktop file so the portal can resolve our app ID.
// ---------------------------------------------------------------------------
static void writeDesktopFile(const QString &path, const QString &execPath)
{
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Could not write desktop file:" << path;
        return;
    }
    QTextStream(& f)
    << "[Desktop Entry]\n"
    << "Type=Application\n"
    << "Name=Info Overlay\n"
    << "Exec="       << execPath << "\n"
    << "Icon=utilities-system-monitor\n"
    << "Categories=Utility;\n"
    << "NoDisplay=true\n"
    << "StartupNotify=false\n"
    << "X-KDE-StartupNotify=false\n";

    qDebug() << "Wrote desktop file:" << path;
}

// Installs the .desktop file in ~/.local/share/applications/ (where the
// portal looks) and next to the executable (for AppImage portability).
static void ensureDesktopFiles(const QString &execPath)
{
    const QString appsDir  = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation);
    const QString execDir  = QFileInfo(execPath).absolutePath();
    const QString fileName = QStringLiteral("info_overlay.desktop");

    QDir().mkpath(appsDir);
    writeDesktopFile(appsDir + '/' + fileName, execPath);

    if (execDir != appsDir)
        writeDesktopFile(execDir + '/' + fileName, execPath);
}

// ---------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    qputenv("QT_QPA_PLATFORM", "wayland");

    QGuiApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("info_overlay"));
    app.setOrganizationDomain(QStringLiteral("local.project"));
    app.setApplicationDisplayName(QStringLiteral("Info Overlay"));

    // Prefer $APPIMAGE so the .desktop Exec line points at the AppImage itself.
    const QString execPath = [] {
        QString p = QString::fromLocal8Bit(qgetenv("APPIMAGE"));
        return p.isEmpty() ? QCoreApplication::applicationFilePath() : p;
    }();
    ensureDesktopFiles(execPath);
    app.setDesktopFileName(QStringLiteral("info_overlay"));

    // --- Load QML ---
    QQmlApplicationEngine engine;
    auto *shortcuts = new PortalShortcuts(nullptr, &engine);
    engine.rootContext()->setContextProperty("portalShortcuts", shortcuts);
    engine.loadFromModule("info_overlay", "Main");

    if (engine.rootObjects().isEmpty()) {
        qCritical() << "Failed to load QML root object — exiting.";
        return -1;
    }

    // --- Wire up the window ---
    auto *window = qobject_cast<QQuickWindow *>(engine.rootObjects().first());
    if (!window) {
        qCritical() << "Root QML object is not a QQuickWindow — exiting.";
        return -1;
    }

    shortcuts->setWindow(window);
    new OverlayMaskHandler(window, window);
    window->show();

    return app.exec();
}
