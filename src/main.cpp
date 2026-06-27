#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QStandardPaths>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QFileInfo>
#include <QDebug>

#include "OverlayMaskHandler.h"
#include "PortalShortcuts.h"

static void writeDesktopFile(const QString &filePath, const QString &execPath)
{
    QFile f(filePath);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Could not write desktop file to" << filePath;
        return;
    }
    QTextStream s(&f);
    s << "[Desktop Entry]\n"
    << "Type=Application\n"
    << "Name=Info Overlay\n"
    << "Exec=" << execPath << "\n"
    << "Icon=utilities-system-monitor\n"
    << "Categories=Utility;\n"
    << "NoDisplay=true\n"
    << "StartupNotify=false\n"
    << "X-KDE-StartupNotify=false\n";
    qDebug() << "Wrote desktop file:" << filePath;
}

static void ensureDesktopFiles(const QString &execPath)
{
    const QString fileName = QStringLiteral("info_overlay.desktop");

    // 1. ~/.local/share/applications/ — where the portal looks for app IDs
    const QString appsDir = QStandardPaths::writableLocation(
        QStandardPaths::ApplicationsLocation);
    QDir().mkpath(appsDir);
    writeDesktopFile(appsDir + QLatin1Char('/') + fileName, execPath);

    // 2. Same directory as the executable/AppImage — for portability
    const QString execDir = QFileInfo(execPath).absolutePath();
    if (execDir != appsDir)
        writeDesktopFile(execDir + QLatin1Char('/') + fileName, execPath);
}

int main(int argc, char *argv[])
{
    qputenv("QT_QPA_PLATFORM", "wayland");

    QGuiApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("info_overlay"));
    app.setOrganizationDomain(QStringLiteral("local.project"));
    app.setApplicationDisplayName(QStringLiteral("Info Overlay"));

    // Resolve real executable path (handles AppImage via $APPIMAGE env var)
    QString execPath = QString::fromLocal8Bit(qgetenv("APPIMAGE"));
    if (execPath.isEmpty())
        execPath = QFileInfo(QString::fromLocal8Bit(argv[0])).absoluteFilePath();

    ensureDesktopFiles(execPath);
    app.setDesktopFileName(QStringLiteral("info_overlay"));

    QQmlApplicationEngine engine;
    engine.loadFromModule("info_overlay", "Main");
    if (engine.rootObjects().isEmpty())
        return -1;

    QQuickWindow *window = qobject_cast<QQuickWindow *>(engine.rootObjects().first());
    if (window) {
        new OverlayMaskHandler(window, window);
        new PortalShortcuts(window, window);
        window->show();
    }

    return app.exec();
}
