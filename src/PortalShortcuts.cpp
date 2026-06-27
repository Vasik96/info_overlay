#include "PortalShortcuts.h"
#include "OverlayMaskHandler.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusMetaType>
#include <QDBusPendingCall>
#include <QDBusPendingCallWatcher>
#include <QDBusPendingReply>
#include <QDebug>

static constexpr char kPortalService[]   = "org.freedesktop.portal.Desktop";
static constexpr char kPortalPath[]      = "/org/freedesktop/portal/desktop";
static constexpr char kPortalInterface[] = "org.freedesktop.portal.GlobalShortcuts";
static constexpr char kRequestInterface[]= "org.freedesktop.portal.Request";

// ---------------------------------------------------------------------------
PortalShortcuts::PortalShortcuts(QQuickWindow *window, QObject *parent)
: QObject(parent), m_window(window)
{
    qDebug() << "[PortalShortcuts] Constructor. window =" << window;

    qDBusRegisterMetaType<ShortcutEntry>();
    qDBusRegisterMetaType<ShortcutList>();

    m_senderName = QDBusConnection::sessionBus().baseService();
    m_senderName.remove(':');
    m_senderName.replace('.', '_');
    qDebug() << "[PortalShortcuts] D-Bus sender name (munged):" << m_senderName;
}

// ---------------------------------------------------------------------------
// Core helper: pre-subscribe to the portal Response signal for 'token', then
// fire an async GlobalShortcuts method call.  All portal methods return (o)
// synchronously; the real payload arrives via the Response signal later.
// ---------------------------------------------------------------------------
void PortalShortcuts::portalCall(const QString      &method,
                                 const QString      &token,
                                 const QList<QVariant> &args,
                                 const char         *responseSlot)
{
    const QString reqPath = QStringLiteral(
        "/org/freedesktop/portal/desktop/request/%1/%2").arg(m_senderName, token);

        qDebug() << "[PortalShortcuts] portalCall:" << method
        << "| token:" << token << "| reqPath:" << reqPath;

        bool ok = QDBusConnection::sessionBus().connect(
            kPortalService, reqPath, kRequestInterface, QStringLiteral("Response"),
                                                        this, responseSlot);
        if (!ok)
            qWarning() << "[PortalShortcuts] Failed to pre-subscribe Response on" << reqPath;

    QDBusMessage msg = QDBusMessage::createMethodCall(
        kPortalService, kPortalPath, kPortalInterface, method);
    for (const QVariant &arg : args)
        msg << arg;

    QDBusPendingCall call = QDBusConnection::sessionBus().asyncCall(msg);
    auto *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this,
            [method](QDBusPendingCallWatcher *w) {
                w->deleteLater();
                QDBusPendingReply<QDBusObjectPath> reply = *w;
                if (reply.isError())
                    qWarning() << "[PortalShortcuts]" << method << "call failed:"
                    << reply.error().name() << reply.error().message();
                else
                    qDebug() << "[PortalShortcuts]" << method
                    << "accepted, request path:" << reply.value().path();
            });
}

// ---------------------------------------------------------------------------
// Step 1 – Create a portal session.
// ---------------------------------------------------------------------------
void PortalShortcuts::initPortalSession()
{
    qDebug() << "[PortalShortcuts] initPortalSession()";

    QDBusConnection bus = QDBusConnection::sessionBus();
    if (!bus.isConnected()) {
        qWarning() << "[PortalShortcuts] Session D-Bus not connected.";
        return;
    }

    // Subscribe to ShortcutsChanged so live reassignments update the label.
    bool ok = bus.connect(kPortalService, kPortalPath, kPortalInterface,
                          QStringLiteral("ShortcutsChanged"),
                          this, SLOT(handleShortcutsChanged(QDBusObjectPath,ShortcutList)));
    qDebug() << "[PortalShortcuts] ShortcutsChanged subscription:" << (ok ? "ok" : "FAILED");

    const QString token = QStringLiteral("info_overlay_req");
    QVariantMap opts;
    opts[QStringLiteral("session_handle_token")] = QStringLiteral("info_overlay_session");
    opts[QStringLiteral("handle_token")]         = token;

    portalCall(QStringLiteral("CreateSession"), token,
               { opts },
               SLOT(handleSessionCreatedResponse(uint,QVariantMap)));
}

// ---------------------------------------------------------------------------
// Step 2 – Session ready → bind shortcuts.
// ---------------------------------------------------------------------------
void PortalShortcuts::handleSessionCreatedResponse(uint code, const QVariantMap &results)
{
    qDebug() << "[PortalShortcuts] handleSessionCreatedResponse code=" << code << results;
    if (code != 0) { qWarning() << "[PortalShortcuts] Session creation failed. Code:" << code; return; }

    m_sessionPath = results.value(QStringLiteral("session_handle")).toString();
    if (m_sessionPath.isEmpty()) {
        qWarning() << "[PortalShortcuts] session_handle missing in results:" << results;
        return;
    }
    qDebug() << "[PortalShortcuts] Session handle:" << m_sessionPath;
    registerShortcuts(QDBusObjectPath(m_sessionPath));
}

// ---------------------------------------------------------------------------
// Step 3 – Bind shortcuts (shows the KDE assignment dialog).
// ---------------------------------------------------------------------------
void PortalShortcuts::registerShortcuts(const QDBusObjectPath &sessionHandle)
{
    qDebug() << "[PortalShortcuts] registerShortcuts() session:" << sessionHandle.path();

    ShortcutList shortcuts;
    shortcuts.append({ QStringLiteral("toggle_overlay"), {
        { QStringLiteral("description"),       QStringLiteral("Toggle Info Overlay Visibility") },
                     { QStringLiteral("preferred_trigger"), QStringLiteral("Meta+F12") }
    }});

    const QString token = QStringLiteral("info_overlay_bind_req");
    QVariantMap opts;
    opts[QStringLiteral("handle_token")] = token;

    portalCall(QStringLiteral("BindShortcuts"), token,
               { QVariant::fromValue(sessionHandle),
                   QVariant::fromValue(shortcuts),
               QString{},   // parent window handle (empty = portal decides placement)
    opts },
    SLOT(handleBindShortcutsResponse(uint,QVariantMap)));
}

// ---------------------------------------------------------------------------
// Step 4 – User confirmed binding → subscribe to Activated + fetch label.
// ---------------------------------------------------------------------------
void PortalShortcuts::handleBindShortcutsResponse(uint code, const QVariantMap &results)
{
    Q_UNUSED(results)
    qDebug() << "[PortalShortcuts] handleBindShortcutsResponse code=" << code;
    if (code != 0) { qWarning() << "[PortalShortcuts] Shortcut binding rejected. Code:" << code; return; }

    bool ok = QDBusConnection::sessionBus().connect(
        kPortalService, kPortalPath, kPortalInterface, QStringLiteral("Activated"),
                                                    this, SLOT(handleActivated(QDBusObjectPath,QString,qulonglong,QVariantMap)));
    qDebug() << "[PortalShortcuts] Activated subscription:" << (ok ? "ok" : "FAILED");

    listShortcuts();
}

// ---------------------------------------------------------------------------
// Step 5 – Fetch the current shortcut list to populate the UI label.
// ---------------------------------------------------------------------------
void PortalShortcuts::listShortcuts()
{
    qDebug() << "[PortalShortcuts] listShortcuts() session:" << m_sessionPath;

    const QString token = QStringLiteral("info_overlay_list_req");
    QVariantMap opts;
    opts[QStringLiteral("handle_token")] = token;

    portalCall(QStringLiteral("ListShortcuts"), token,
               { QVariant::fromValue(QDBusObjectPath(m_sessionPath)), opts },
               SLOT(handleListShortcutsResponse(uint,QVariantMap)));
}

// ---------------------------------------------------------------------------
// Step 5b – ListShortcuts Response → update the label.
// ---------------------------------------------------------------------------
void PortalShortcuts::handleListShortcutsResponse(uint code, const QVariantMap &results)
{
    qDebug() << "[PortalShortcuts] handleListShortcutsResponse code=" << code;
    if (code != 0) { qWarning() << "[PortalShortcuts] ListShortcuts denied. Code:" << code; return; }

    if (!results.contains(QStringLiteral("shortcuts"))) {
        qWarning() << "[PortalShortcuts] 'shortcuts' key missing in results:" << results;
        return;
    }
    auto list = qdbus_cast<ShortcutList>(results.value(QStringLiteral("shortcuts")));
    qDebug() << "[PortalShortcuts] ListShortcuts returned" << list.size() << "shortcut(s).";
    updateShortcutLabel(list);
}

// ---------------------------------------------------------------------------
// Fired by the compositor when the bound shortcut is pressed.
// ---------------------------------------------------------------------------
void PortalShortcuts::handleActivated(const QDBusObjectPath &session,
                                      const QString &id,
                                      qulonglong timestamp,
                                      const QVariantMap &options)
{
    Q_UNUSED(options)
    qDebug() << "[PortalShortcuts] handleActivated id=" << id
    << "session=" << session.path() << "ts=" << timestamp;

    if (session.path() != m_sessionPath) return;

    if (id == QStringLiteral("toggle_overlay")) {
        if (!m_window) { qWarning() << "[PortalShortcuts] m_window is null."; return; }
        auto *card = m_window->findChild<QObject *>(QStringLiteral("overlayCard"));
        if (!card) { qWarning() << "[PortalShortcuts] overlayCard not found."; return; }

        bool nowVisible = !card->property("visible").toBool();
        qDebug() << "[PortalShortcuts] Toggling overlayCard visible ->" << nowVisible;
        card->setProperty("visible", nowVisible);
        OverlayMaskHandler::updateOverlayMask(m_window);
    } else {
        qDebug() << "[PortalShortcuts] Unhandled shortcut id:" << id;
    }
}

// ---------------------------------------------------------------------------
// Live shortcut reassignment from the portal.
// ---------------------------------------------------------------------------
void PortalShortcuts::handleShortcutsChanged(QDBusObjectPath session, ShortcutList list)
{
    qDebug() << "[PortalShortcuts] handleShortcutsChanged session=" << session.path()
    << "count=" << list.size();
    if (session.path() == m_sessionPath)
        updateShortcutLabel(list);
}

// ---------------------------------------------------------------------------
// Extract the trigger_description for "toggle_overlay" and notify QML.
// ---------------------------------------------------------------------------
void PortalShortcuts::updateShortcutLabel(const ShortcutList &list)
{
    for (const auto &entry : list) {
        if (entry.first != QStringLiteral("toggle_overlay")) continue;

        QString trigger = entry.second.value(QStringLiteral("trigger_description")).toString();
        qDebug() << "[PortalShortcuts] updateShortcutLabel: trigger_description =" << trigger;

        if (trigger.isEmpty())
            qWarning() << "[PortalShortcuts] trigger_description is empty (no key assigned yet).";

        if (m_shortcutLabel != trigger) {
            m_shortcutLabel = trigger;
            emit shortcutLabelChanged();
        }
        return;
    }
    qWarning() << "[PortalShortcuts] 'toggle_overlay' entry not found in shortcut list.";
}

// ---------------------------------------------------------------------------
void PortalShortcuts::setWindow(QQuickWindow *window)
{
    qDebug() << "[PortalShortcuts] setWindow() old=" << m_window << "new=" << window;
    if (m_window == window) return;
    m_window = window;
    if (!m_window) return;

    if (m_window->isVisible()) {
        initPortalSession();
    } else {
        connect(m_window, &QQuickWindow::visibleChanged, this, [this]() {
            if (m_window && m_window->isVisible()) {
                disconnect(m_window, &QQuickWindow::visibleChanged, this, nullptr);
                qDebug() << "[PortalShortcuts] Window became visible, starting portal session.";
                initPortalSession();
            }
        });
    }
}
