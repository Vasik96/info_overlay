#include "PortalShortcuts.h"
#include "OverlayMaskHandler.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusMetaType>
#include <QDBusPendingCall>
#include <QDBusPendingCallWatcher>
#include <QDBusPendingReply>
#include <QDebug>
// ---------------------------------------------------------------------------
// Helper: produce a parent-window handle string for the portal BindShortcuts
// call so KDE can associate the assignment dialog with the right window.
//
// Getting a real xdg-foreign wl_surface handle requires private QtWayland
// headers (QWaylandWindow) that aren't guaranteed to be installed.  For a
// layer-shell overlay the portal dialog appears regardless of this value, so
// we pass an empty string which the spec explicitly allows.
// ---------------------------------------------------------------------------
static QString parentWindowHandle(QQuickWindow * /*window*/)
{
    return {};
}


PortalShortcuts::PortalShortcuts(QQuickWindow *window, QObject *parent)
: QObject(parent), m_window(window)
{
    // Register our custom D-Bus metatypes once at construction time.
    qDBusRegisterMetaType<ShortcutEntry>();
    qDBusRegisterMetaType<ShortcutList>();

    // Pre-compute the munged sender name used to predict request object paths.
    m_senderName = QDBusConnection::sessionBus().baseService();
    m_senderName.remove(':');
    m_senderName.replace('.', '_');

    // WAIT for the window to actually be visible on screen before asking Wayland for a popup!
    // This bypasses Wayland's strict anti-focus-stealing protections.
    if (m_window->isVisible()) {
        initPortalSession();
    } else {
        connect(m_window, &QQuickWindow::visibleChanged, this, [this]() {
            if (m_window->isVisible()) {
                // Disconnect immediately so this only runs once
                disconnect(m_window, &QQuickWindow::visibleChanged, this, nullptr);
                initPortalSession();
            }
        });
    }
}

// ---------------------------------------------------------------------------
// Builds the path the portal WILL use for a request, given our handle token.
// Format: /org/freedesktop/portal/desktop/request/<sender>/<token>
// We need this to subscribe BEFORE sending the call to avoid the race where
// the portal responds before our watcher is connected.
// ---------------------------------------------------------------------------
QString PortalShortcuts::buildRequestPath(const QString &token) const
{
    return QStringLiteral("/org/freedesktop/portal/desktop/request/%1/%2")
    .arg(m_senderName, token);
}


// ---------------------------------------------------------------------------
// Step 1 – Create a portal session.
// We subscribe to the Response signal on the predicted request path BEFORE
// making the async call so we never miss a fast portal response.
// ---------------------------------------------------------------------------
void PortalShortcuts::initPortalSession()
{
    QDBusConnection bus = QDBusConnection::sessionBus();
    if (!bus.isConnected()) {
        qWarning() << "Session D-Bus is not connected.";
        return;
    }

    const QString sessionToken = QStringLiteral("info_overlay_session");
    const QString requestToken = QStringLiteral("info_overlay_req");

    QVariantMap options;
    options[QStringLiteral("session_handle_token")] = sessionToken;
    options[QStringLiteral("handle_token")]         = requestToken;

    // --- Subscribe BEFORE the call to avoid the response-race ---
    const QString reqPath = buildRequestPath(requestToken);
    qDebug() << "Pre-subscribing to session request path:" << reqPath;

    bool ok = bus.connect(
        QStringLiteral("org.freedesktop.portal.Desktop"),
                          reqPath,
                          QStringLiteral("org.freedesktop.portal.Request"),
                          QStringLiteral("Response"),
                          this, SLOT(handleSessionCreatedResponse(uint,QVariantMap))
    );
    if (!ok)
        qWarning() << "Failed to connect to session Response signal on" << reqPath;

    QDBusMessage msg = QDBusMessage::createMethodCall(
        QStringLiteral("org.freedesktop.portal.Desktop"),
                                                      QStringLiteral("/org/freedesktop/portal/desktop"),
                                                      QStringLiteral("org.freedesktop.portal.GlobalShortcuts"),
                                                      QStringLiteral("CreateSession")
    );
    msg << options;

    QDBusPendingCall call = bus.asyncCall(msg);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this,
            [this](QDBusPendingCallWatcher *w) {
                w->deleteLater();
                QDBusPendingReply<QDBusObjectPath> reply = *w;
                if (reply.isError()) {
                    qWarning() << "CreateSession method call failed:"
                    << reply.error().name()
                    << reply.error().message();
                } else {
                    qDebug() << "CreateSession call accepted, request path:"
                    << reply.value().path();
                }
            });
}


// ---------------------------------------------------------------------------
// Step 2 – The portal confirmed the session handle.  Now bind our shortcuts.
// ---------------------------------------------------------------------------
void PortalShortcuts::handleSessionCreatedResponse(uint responseCode,
                                                   const QVariantMap &results)
{
    if (responseCode != 0) {
        qWarning() << "Portal session creation denied/cancelled. Code:" << responseCode;
        return;
    }

    if (!results.contains(QStringLiteral("session_handle"))) {
        qWarning() << "Portal response missing session_handle key. Results:" << results;
        return;
    }

    m_sessionPath = results.value(QStringLiteral("session_handle")).toString();
    qDebug() << "Portal session created successfully:" << m_sessionPath;

    registerShortcuts(QDBusObjectPath(m_sessionPath));
}


// ---------------------------------------------------------------------------
// Step 3 – Call BindShortcuts with a correctly-typed a(sa{sv}) argument and
// a valid parent-window handle so KDE shows the assignment dialog.
// Again we pre-subscribe to the Response signal before the async call.
// ---------------------------------------------------------------------------
void PortalShortcuts::registerShortcuts(const QDBusObjectPath &sessionHandle)
{
    QDBusConnection bus = QDBusConnection::sessionBus();

    const QString bindRequestToken = QStringLiteral("info_overlay_bind_req");

    // Build shortcut list with correct D-Bus-registered type
    ShortcutList shortcuts;
    QVariantMap props;
    props[QStringLiteral("description")]        = QStringLiteral("Toggle Info Overlay Visibility");
    props[QStringLiteral("preferred_trigger")]  = QStringLiteral("Meta+F12");
    shortcuts.append({ QStringLiteral("toggle_overlay"), props });

    // Window handle for KDE so it knows which window is requesting and can
    // properly raise the dialog.  Empty string often causes silent suppression.
    const QString parentHandle = parentWindowHandle(m_window);
    if (parentHandle.isEmpty())
        qDebug() << "No Wayland activation token available; dialog may not surface.";
    else
        qDebug() << "Using parent window handle:" << parentHandle;

    QVariantMap bindOptions;
    bindOptions[QStringLiteral("handle_token")] = bindRequestToken;

    // --- Pre-subscribe before the call ---
    const QString bindReqPath = buildRequestPath(bindRequestToken);
    qDebug() << "Pre-subscribing to bind request path:" << bindReqPath;

    bool ok = QDBusConnection::sessionBus().connect(
        QStringLiteral("org.freedesktop.portal.Desktop"),
                                                    bindReqPath,
                                                    QStringLiteral("org.freedesktop.portal.Request"),
                                                    QStringLiteral("Response"),
                                                    this, SLOT(handleBindShortcutsResponse(uint,QVariantMap))
    );
    if (!ok)
        qWarning() << "Failed to connect to bind Response signal on" << bindReqPath;

    QDBusMessage bindCall = QDBusMessage::createMethodCall(
        QStringLiteral("org.freedesktop.portal.Desktop"),
                                                           QStringLiteral("/org/freedesktop/portal/desktop"),
                                                           QStringLiteral("org.freedesktop.portal.GlobalShortcuts"),
                                                           QStringLiteral("BindShortcuts")
    );

    // Argument order per spec:
    //   session_handle (o), shortcuts (a(sa{sv})), parent_window (s), options (a{sv})
    bindCall << QVariant::fromValue(sessionHandle)
    << QVariant::fromValue(shortcuts)
    << parentHandle
    << bindOptions;

    QDBusPendingCall pending = bus.asyncCall(bindCall);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pending, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this,
            [](QDBusPendingCallWatcher *w) {
                w->deleteLater();
                QDBusPendingReply<QDBusObjectPath> reply = *w;
                if (reply.isError()) {
                    qWarning() << "BindShortcuts method call failed:"
                    << reply.error().name()
                    << reply.error().message();
                } else {
                    qDebug() << "BindShortcuts call accepted, request path:"
                    << reply.value().path();
                }
            });
}


// ---------------------------------------------------------------------------
// Step 4 – User confirmed the shortcut assignment in the KDE dialog.
// Now hook up the global Activated signal so we receive trigger events.
// ---------------------------------------------------------------------------
void PortalShortcuts::handleBindShortcutsResponse(uint responseCode,
                                                  const QVariantMap &results)
{
    Q_UNUSED(results)

    if (responseCode != 0) {
        qWarning() << "User rejected or dismissed the shortcut binding dialog. Code:"
        << responseCode;
        return;
    }

    qDebug() << "Shortcuts bound successfully. Subscribing to Activated signal.";

    bool connected = QDBusConnection::sessionBus().connect(
        QStringLiteral("org.freedesktop.portal.Desktop"),
                                                           QStringLiteral("/org/freedesktop/portal/desktop"),
                                                           QStringLiteral("org.freedesktop.portal.GlobalShortcuts"),
                                                           QStringLiteral("Activated"),
                                                           this,
                                                           SLOT(handleActivated(QDBusObjectPath,QString,qulonglong,QVariantMap))
    );

    if (connected)
        qDebug() << "Listening for global shortcut activations.";
    else
        qWarning() << "Failed to connect to Activated signal.";
}


// ---------------------------------------------------------------------------
// Fired by the compositor whenever our bound shortcut is triggered.
// ---------------------------------------------------------------------------
void PortalShortcuts::handleActivated(const QDBusObjectPath &targetSession,
                                      const QString &shortcutId,
                                      qulonglong timestamp,
                                      const QVariantMap &options)
{
    Q_UNUSED(options)

    if (targetSession.path() != m_sessionPath)
        return;

    if (shortcutId == QStringLiteral("toggle_overlay")) {
        QObject *card = m_window->findChild<QObject *>(QStringLiteral("overlayCard"));
        if (card) {
            bool visible = card->property("visible").toBool();
            card->setProperty("visible", !visible);
            OverlayMaskHandler::updateOverlayMask(m_window);
            qDebug() << "Overlay toggled via global shortcut. Timestamp:" << timestamp;
        }
    }
}
