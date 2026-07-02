#pragma once

#include <QObject>
#include <QProcess>
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <QDebug>

class AppLauncher : public QObject {
    Q_OBJECT
public:
    explicit AppLauncher(QObject *parent = nullptr) : QObject(parent) {}

    Q_INVOKABLE void launchApp(const QString &appName) {
        QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
        QString filePath = configDir + "/config.json";

        QString desktopPath = QString("/usr/share/applications/%1.desktop").arg(appName.toLower());

        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly)) {
            QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
            QJsonObject obj = doc.object();
            if (obj.contains(appName)) {
                desktopPath = obj.value(appName).toString();
            }
        } else {
            QDir().mkpath(configDir);
            if (file.open(QIODevice::WriteOnly)) {
                QJsonObject defaultMap;
                defaultMap["Discord"] = "/usr/share/applications/discord.desktop";
                defaultMap["Firefox"] = "/usr/share/applications/firefox.desktop";
                defaultMap["Steam"] = "/usr/share/applications/steam.desktop";
                file.write(QJsonDocument(defaultMap).toJson());
            }
        }

        // --- HARDCODED SPECIAL HANDLING FOR DISCORD ---
        if (appName.toLower().contains("discord")) {
            qDebug() << "Applying hardcoded Electron isolation routine for Discord...";

            // We use 'nohup' to ignore hangup signals when our overlay closes,
            // and force-redirect all standard/error streams to the system blackhole.
            QString shellCommand = QString("nohup gtk-launch %1 > /dev/null 2>&1 &")
            .arg(QFileInfo(desktopPath).baseName());
            // extracts 'discord-ptb' out of the full path

            QProcess::startDetached("/bin/sh", QStringList() << "-c" << shellCommand);
            return;
        }

        // --- STANDARD LAUNCH FOR EVERYTHING ELSE ---
        QStringList arguments;
        arguments << "launch" << desktopPath;
        qint64 pid = 0;
        QProcess::startDetached("gio", arguments, QDir::homePath(), &pid);
    }
};
