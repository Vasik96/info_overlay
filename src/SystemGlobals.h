#pragma once
#include <QObject>
#include <QString>
#include <QTimer>

class SystemGlobals : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString cpuUsage READ cpuUsage NOTIFY statsChanged)
    Q_PROPERTY(QString cpuTemp READ cpuTemp NOTIFY statsChanged)
    Q_PROPERTY(QString ramUsage READ ramUsage NOTIFY statsChanged)

public:
    explicit SystemGlobals(QObject *parent = nullptr);

    QString cpuUsage() const { return m_cpuUsage; }
    QString cpuTemp() const { return m_cpuTemp; }
    QString ramUsage() const { return m_ramUsage; }

signals:
    void statsChanged();

private slots:
    void updateStats();

private:
    QTimer *m_pollingTimer = nullptr;

    // UI Cache Strings
    QString m_cpuUsage = QStringLiteral("--%");
    QString m_cpuTemp  = QStringLiteral("--°C");
    QString m_ramUsage = QStringLiteral("-- / -- GB");

    // Internal trackers for calculating CPU delta steps
    unsigned long long m_lastUser = 0;
    unsigned long long m_lastUserNice = 0;
    unsigned long long m_lastSystem = 0;
    unsigned long long m_lastIdle = 0;

    void calculateCpuUsage();
    void calculateCpuTemp();
    void calculateRamUsage();
};
