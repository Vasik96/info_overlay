#pragma once
#include <QObject>
#include <QQuickWindow>
#include <QDBusObjectPath>
#include <QDBusArgument>
#include <QVariantMap>

typedef QPair<QString, QVariantMap> ShortcutEntry;
typedef QList<ShortcutEntry>        ShortcutList;
Q_DECLARE_METATYPE(ShortcutEntry)
Q_DECLARE_METATYPE(ShortcutList)

inline QDBusArgument &operator<<(QDBusArgument &arg, const ShortcutEntry &e)
{
    arg.beginStructure(); arg << e.first << e.second; arg.endStructure();
    return arg;
}
inline const QDBusArgument &operator>>(const QDBusArgument &arg, ShortcutEntry &e)
{
    arg.beginStructure(); arg >> e.first >> e.second; arg.endStructure();
    return arg;
}
inline QDBusArgument &operator<<(QDBusArgument &arg, const ShortcutList &list)
{
    arg.beginArray(qMetaTypeId<ShortcutEntry>());
    for (const auto &e : list) arg << e;
    arg.endArray();
    return arg;
}
inline const QDBusArgument &operator>>(const QDBusArgument &arg, ShortcutList &list)
{
    arg.beginArray();
    while (!arg.atEnd()) { ShortcutEntry e; arg >> e; list.append(e); }
    arg.endArray();
    return arg;
}

class PortalShortcuts : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString shortcutLabel READ shortcutLabel NOTIFY shortcutLabelChanged)

public:
    explicit PortalShortcuts(QQuickWindow *window, QObject *parent = nullptr);
    void    setWindow(QQuickWindow *window);
    QString shortcutLabel() const { return m_shortcutLabel; }

signals:
    void shortcutLabelChanged();

private slots:
    void handleSessionCreatedResponse(uint code, const QVariantMap &results);
    void handleBindShortcutsResponse (uint code, const QVariantMap &results);
    void handleListShortcutsResponse (uint code, const QVariantMap &results);
    void handleActivated(const QDBusObjectPath &session, const QString &id,
                         qulonglong timestamp, const QVariantMap &options);
    void handleShortcutsChanged(QDBusObjectPath session, ShortcutList list);

private:
    void initPortalSession();
    void registerShortcuts(const QDBusObjectPath &sessionHandle);
    void listShortcuts();

    // Pre-subscribe to the portal Response signal for 'token', then dispatch
    // 'method' with 'args' on the GlobalShortcuts interface.  Returns the
    // pending call watcher so the caller can log the immediate (o) reply.
    void portalCall(const QString &method, const QString &token,
                    const QList<QVariant> &args, const char *responseSlot);

    void updateShortcutLabel(const ShortcutList &list);

    QQuickWindow *m_window      = nullptr;
    QString       m_sessionPath;
    QString       m_senderName;
    QString       m_shortcutLabel = QStringLiteral("unset");
};
