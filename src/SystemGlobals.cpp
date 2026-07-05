#include "SystemGlobals.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDebug>
#include <iostream>

SystemGlobals::SystemGlobals(QObject *parent)
: QObject(parent)
{
    m_pollingTimer = new QTimer(this);
    m_pollingTimer->setInterval(2000); // 2-second heartbeat is perfect for low overhead

    connect(m_pollingTimer, &QTimer::timeout, this, &SystemGlobals::updateStats);
    m_pollingTimer->start();

    QTimer::singleShot(1100, this, &SystemGlobals::updateStats);
}

void SystemGlobals::updateStats()
{
    calculateCpuUsage();
    calculateCpuTemp();
    calculateRamUsage();

    emit statsChanged();
}

// ---------------------------------------------------------------------------
// CPU USAGE ACCELERATOR (Parses /proc/stat)
// ---------------------------------------------------------------------------
void SystemGlobals::calculateCpuUsage()
{
    QFile file(QStringLiteral("/proc/stat"));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

    QTextStream stream(&file);
    QString line = stream.readLine(); // The first line "cpu  ..." represents the overall aggregate
    file.close();

    if (!line.startsWith(QStringLiteral("cpu"))) return;

    QStringList tokens = line.split(QLatin1Char(' '), Qt::SkipEmptyParts);
    if (tokens.size() < 5) return;

    // Core cycles fields - Cleaned variable names
    unsigned long long user     = tokens.at(1).toULongLong();
    unsigned long long nice     = tokens.at(2).toULongLong();
    unsigned long long system   = tokens.at(3).toULongLong();
    unsigned long long idle     = tokens.at(4).toULongLong();

    unsigned long long totalAll = user + nice + system + idle;
    unsigned long long totalWork = user + nice + system;

    unsigned long long lastTotalAll = m_lastUser + m_lastUserNice + m_lastSystem + m_lastIdle;
    unsigned long long lastTotalWork = m_lastUser + m_lastUserNice + m_lastSystem;

    unsigned long long deltaTotal = totalAll - lastTotalAll;
    unsigned long long deltaWork = totalWork - lastTotalWork;

    if (deltaTotal > 0) {
        double percent = (static_cast<double>(deltaWork) / deltaTotal) * 100.0;
        m_cpuUsage = QString::number(qRound(percent)) + QStringLiteral("%");
    }

    // Save history cycles for the next 2-second delta comparison step
    m_lastUser = user;
    m_lastUserNice = nice;
    m_lastSystem = system;
    m_lastIdle = idle;
}

// ---------------------------------------------------------------------------
// CPU TEMPERATURE ACCELERATOR (Parses hardware thermal zones)
// ---------------------------------------------------------------------------
void SystemGlobals::calculateCpuTemp()
{
    int highestTemp = -1;

    // Scan all available thermal zones to find the actual active CPU package
    QDir dir(QStringLiteral("/sys/class/thermal/"));
    QStringList zones = dir.entryList({QStringLiteral("thermal_zone*")}, QDir::Dirs);

    for (const QString &zone : zones) {
        // Check the type of this zone
        QFile typeFile(dir.absoluteFilePath(zone + QStringLiteral("/type")));
        if (!typeFile.open(QIODevice::ReadOnly | QIODevice::Text)) continue;
        QString typeStr = typeFile.readAll().toLower();
        typeFile.close();

        // Skip known battery, GPU, or wireless card thermal zones
        if (typeStr.contains(QStringLiteral("iwlwifi")) || typeStr.contains(QStringLiteral("amdgpu"))) {
            continue;
        }

        // Read the temperature value
        QFile tempFile(dir.absoluteFilePath(zone + QStringLiteral("/temp")));
        if (!tempFile.open(QIODevice::ReadOnly | QIODevice::Text)) continue;
        int rawTemp = tempFile.readAll().trimmed().toInt();
        tempFile.close();

        // Linux kernel multiplies temps by 1000 (e.g., 55000 = 55°C)
        int tempC = rawTemp / 1000;

        // Filter out obviously glitched values (like 0°C, 16°C ambient, or 120°C errors)
        // We pick the highest valid reading matching core CPU operation ranges
        if (tempC > 25 && tempC < 105) {
            if (tempC > highestTemp) {
                highestTemp = tempC;
            }
        }
    }

    // Fallback directly to hwmon if thermal zones didn't yield a core reading
    if (highestTemp == -1) {
        QDir hwmonDir(QStringLiteral("/sys/class/hwmon/"));
        QStringList hwmons = hwmonDir.entryList({QStringLiteral("hwmon*")}, QDir::Dirs);
        for (const QString &hw : hwmons) {
            QFile nameFile(hwmonDir.absoluteFilePath(hw + QStringLiteral("/name")));
            if (!nameFile.open(QIODevice::ReadOnly | QIODevice::Text)) continue;
            QString name = nameFile.readAll().trimmed();
            nameFile.close();

            // coretemp = Intel, k10temp/zenpower = AMD
            if (name.contains(QStringLiteral("coretemp")) || name.contains(QStringLiteral("k10temp"))) {
                QFile tempFile(hwmonDir.absoluteFilePath(hw + QStringLiteral("/temp1_input")));
                if (tempFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                    highestTemp = tempFile.readAll().trimmed().toInt() / 1000;
                    tempFile.close();
                    break;
                }
            }
        }
    }



    if (highestTemp > 0) {
        m_cpuTemp = QString::number(highestTemp) + QStringLiteral("°C");
    } else {
        m_cpuTemp = QStringLiteral("N/A");
    }
}

// ---------------------------------------------------------------------------
// RAM USAGE ACCELERATOR (Parses /proc/meminfo)
// ---------------------------------------------------------------------------

void SystemGlobals::calculateRamUsage()
{
    QFile file(QStringLiteral("/proc/meminfo"));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "SystemGlobals: failed to open /proc/meminfo:" << file.errorString();
        m_ramUsage = QStringLiteral("N/A");
        return;
    }

    const QString contents = QString::fromLocal8Bit(file.readAll());
    file.close();

    unsigned long long totalMem = 0;
    unsigned long long availableMem = 0;

    const QStringList lines = contents.split(QLatin1Char('\n'), Qt::SkipEmptyParts);
    for (const QString &line : lines) {
        QStringList parts = line.split(QLatin1Char(':'));
        if (parts.size() < 2) continue;

        QString key = parts.at(0).trimmed();

        if (key == QStringLiteral("MemTotal")) {
            totalMem = parts.at(1).simplified().split(QLatin1Char(' ')).at(0).toULongLong();
        } else if (key == QStringLiteral("MemAvailable")) {
            availableMem = parts.at(1).simplified().split(QLatin1Char(' ')).at(0).toULongLong();
            break;
        }
    }

    if (totalMem > 0 && totalMem >= availableMem) {
        unsigned long long usedKBytes = totalMem - availableMem;
        double usedGB = static_cast<double>(usedKBytes) / 1048576.0;
        double totalGB = static_cast<double>(totalMem) / 1048576.0;

        m_ramUsage = QString::number(usedGB, 'f', 1) + QStringLiteral(" / ") +
        QString::number(totalGB, 'f', 1) + QStringLiteral(" GB");
    } else {
        m_ramUsage = QStringLiteral("N/A");
    }
}
