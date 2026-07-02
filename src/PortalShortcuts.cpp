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

void PortalShortcuts::portalCall(const QString      &method,
                                 const QString      &token,
                                 const QList<QVariant> &args,
                                 const char         *responseSlot)
{
    const QString reqPath = QStringLiteral(
        "/org/freedesktop/portal/desktop/request/%1/%2").arg(m_senderName, token);

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
            });
}

void PortalShortcuts::initPortalSession()
{
    QDBusConnection bus = QDBusConnection::sessionBus();
    if (!bus.isConnected()) return;

    bus.connect(kPortalService, kPortalPath, kPortalInterface,
                QStringLiteral("ShortcutsChanged"),
                this, SLOT(handleShortcutsChanged(QDBusObjectPath,ShortcutList)));

    const QString token = QStringLiteral("info_overlay_req");
    QVariantMap opts;
    opts[QStringLiteral("session_handle_token")] = QStringLiteral("info_overlay_session");
    opts[QStringLiteral("handle_token")]         = token;

    portalCall(QStringLiteral("CreateSession"), token, { opts }, SLOT(handleSessionCreatedResponse(uint,QVariantMap)));
}

void PortalShortcuts::handleSessionCreatedResponse(uint code, const QVariantMap &results)
{
    if (code != 0) return;
    m_sessionPath = results.value(QStringLiteral("session_handle")).toString();
    registerShortcuts(QDBusObjectPath(m_sessionPath));
}

void PortalShortcuts::registerShortcuts(const QDBusObjectPath &sessionHandle)
{
    ShortcutList shortcuts;
    shortcuts.append({ QStringLiteral("toggle_overlay"), {
        { QStringLiteral("description"),       QStringLiteral("Toggle Info Overlay Visibility") },
                     { QStringLiteral("preferred_trigger"), QStringLiteral("Meta+F12") }
    }});

    const QString token = QStringLiteral("info_overlay_bind_req");
    QVariantMap opts;
    opts[QStringLiteral("handle_token")] = token;

    portalCall(QStringLiteral("BindShortcuts"), token,
               { QVariant::fromValue(sessionHandle), QVariant::fromValue(shortcuts), QString{}, opts },
               SLOT(handleBindShortcutsResponse(uint,QVariantMap)));
}

void PortalShortcuts::handleBindShortcutsResponse(uint code, const QVariantMap &results)
{
    Q_UNUSED(results)
    if (code != 0) return;

    QDBusConnection::sessionBus().connect(
        kPortalService, kPortalPath, kPortalInterface, QStringLiteral("Activated"),
                                          this, SLOT(handleActivated(QDBusObjectPath,QString,qulonglong,QVariantMap)));

    listShortcuts();
}

void PortalShortcuts::listShortcuts()
{
    const QString token = QStringLiteral("info_overlay_list_req");
    QVariantMap opts;
    opts[QStringLiteral("handle_token")] = token;

    portalCall(QStringLiteral("ListShortcuts"), token, { QVariant::fromValue(QDBusObjectPath(m_sessionPath)), opts }, SLOT(handleListShortcutsResponse(uint,QVariantMap)));
}

void PortalShortcuts::handleListShortcutsResponse(uint code, const QVariantMap &results)
{
    if (code != 0 || !results.contains(QStringLiteral("shortcuts"))) return;
    auto list = qdbus_cast<ShortcutList>(results.value(QStringLiteral("shortcuts")));
    updateShortcutLabel(list);
}

// ---------------------------------------------------------------------------
// OPTIMIZED STATE MACHINE: No toggles, no layout re-flips, no flicker.
// ---------------------------------------------------------------------------
void PortalShortcuts::handleActivated(const QDBusObjectPath &session,
                                      const QString &id,
                                      qulonglong timestamp,
                                      const QVariantMap &options)
{
    Q_UNUSED(options) Q_UNUSED(timestamp)
    if (session.path() != m_sessionPath || id != QStringLiteral("toggle_overlay")) return;
    if (!m_window) return;

    auto *card = m_window->findChild<QObject *>(QStringLiteral("overlayCard"));
    if (!card) return;

    // Advance our structural loop state
    m_displayMode = (m_displayMode + 1) % 3;
    emit displayModeChanged();

    qDebug() << "[PortalShortcuts] Hotkey cycle! Transitioning cleanly into state:" << m_displayMode;

    // Apply strict visibility to elements before composition maps update
    if (m_displayMode == 0) {
        card->setProperty("visible", true);       // Interactive: Show panel
    } else if (m_displayMode == 1) {
        card->setProperty("visible", true);       // ViewOnly: Keep open (QML bindings shrink content)
    } else if (m_displayMode == 2) {
        card->setProperty("visible", false);      // Hidden: Drop visibility cleanly
    }

    // Refresh Wayland interaction masks instantly without layout frame stall
    OverlayMaskHandler::updateOverlayMask(m_window);
}

void PortalShortcuts::handleShortcutsChanged(QDBusObjectPath session, ShortcutList list)
{
    if (session.path() == m_sessionPath) updateShortcutLabel(list);
}

void PortalShortcuts::updateShortcutLabel(const ShortcutList &list)
{
    for (const auto &entry : list) {
        if (entry.first != QStringLiteral("toggle_overlay")) continue;
        QString trigger = entry.second.value(QStringLiteral("trigger_description")).toString();
        if (m_shortcutLabel != trigger) {
            m_shortcutLabel = trigger;
            emit shortcutLabelChanged();
        }
        return;
    }
}

void PortalShortcuts::setWindow(QQuickWindow *window)
{
    if (m_window == window) return;
    m_window = window;
    if (!m_window) return;

    if (m_window->isVisible()) {
        initPortalSession();
    } else {
        connect(m_window, &QQuickWindow::visibleChanged, this, [this]() {
            if (m_window && m_window->isVisible()) {
                disconnect(m_window, &QQuickWindow::visibleChanged, this, nullptr);
                initPortalSession();
            }
        });
    }
}
