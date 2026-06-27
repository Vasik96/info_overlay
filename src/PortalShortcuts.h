#pragma once

#include <QObject>
#include <QQuickWindow>
#include <QDBusObjectPath>
#include <QDBusArgument>
#include <QVariantMap>

// Required for correct D-Bus marshalling of a(sa{sv})
typedef QPair<QString, QVariantMap> ShortcutEntry;
typedef QList<ShortcutEntry> ShortcutList;

Q_DECLARE_METATYPE(ShortcutEntry)
Q_DECLARE_METATYPE(ShortcutList)

// Teach Qt's D-Bus how to marshal/unmarshal our shortcut list type
inline QDBusArgument &operator<<(QDBusArgument &arg, const ShortcutEntry &entry)
{
    arg.beginStructure();
    arg << entry.first << entry.second;
    arg.endStructure();
    return arg;
}

inline const QDBusArgument &operator>>(const QDBusArgument &arg, ShortcutEntry &entry)
{
    arg.beginStructure();
    arg >> entry.first >> entry.second;
    arg.endStructure();
    return arg;
}

inline QDBusArgument &operator<<(QDBusArgument &arg, const ShortcutList &list)
{
    arg.beginArray(qMetaTypeId<ShortcutEntry>());
    for (const auto &entry : list)
        arg << entry;
    arg.endArray();
    return arg;
}

inline const QDBusArgument &operator>>(const QDBusArgument &arg, ShortcutList &list)
{
    arg.beginArray();
    while (!arg.atEnd()) {
        ShortcutEntry entry;
        arg >> entry;
        list.append(entry);
    }
    arg.endArray();
    return arg;
}


class PortalShortcuts : public QObject {
    Q_OBJECT

public:
    explicit PortalShortcuts(QQuickWindow *window, QObject *parent = nullptr);

public slots:
    void handleSessionCreatedResponse(uint responseCode, const QVariantMap &results);
    void handleBindShortcutsResponse(uint responseCode, const QVariantMap &results);
    void handleActivated(const QDBusObjectPath &targetSession, const QString &shortcutId,
                         qulonglong timestamp, const QVariantMap &options);

private:
    void initPortalSession();
    void registerShortcuts(const QDBusObjectPath &sessionHandle);

    // Builds the portal request path from a token, matching what the portal constructs
    QString buildRequestPath(const QString &token) const;

    QQuickWindow *m_window;
    QString       m_sessionPath;
    QString       m_senderName; // munged D-Bus unique name, computed once
};
