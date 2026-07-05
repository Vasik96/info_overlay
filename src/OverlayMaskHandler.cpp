#include "OverlayMaskHandler.h"
#include <LayerShellQt/Window>
#include <QRegion>
#include <QDebug>

OverlayMaskHandler::OverlayMaskHandler(QQuickWindow *window, QObject *parent)
: QObject(parent), m_window(window)
{
    qDebug() << "[OverlayMaskHandler] Constructor called. window =" << window;

    if (!m_window) {
        qWarning() << "[OverlayMaskHandler] Window is null; handler is a no-op.";
        return;
    }

    // 1. Setup LayerShell configurations.
    qDebug() << "[OverlayMaskHandler] Fetching LayerShellQt::Window for" << m_window;
    LayerShellQt::Window *layerWindow = LayerShellQt::Window::get(m_window);
    if (layerWindow) {
        qDebug() << "[OverlayMaskHandler] LayerShellQt::Window obtained. Applying initial layer config.";

        auto enforceLayer = [layerWindow]() {
            qDebug() << "[OverlayMaskHandler] enforceLayer() called — setting LayerOverlay + KeyboardInteractivityNone.";
            layerWindow->setLayer(LayerShellQt::Window::LayerOverlay);
            layerWindow->setKeyboardInteractivity(LayerShellQt::Window::KeyboardInteractivityNone);

            layerWindow->setExclusiveZone(-1); // Prevent compositor panels from shifting your center

            // --- FIXED CASTING LINE HERE ---
            layerWindow->setAnchors(LayerShellQt::Window::Anchors(
                LayerShellQt::Window::AnchorTop |
                LayerShellQt::Window::AnchorBottom |
                LayerShellQt::Window::AnchorLeft |
                LayerShellQt::Window::AnchorRight
            ));
        };

        enforceLayer();

        connect(m_window, &QQuickWindow::windowStateChanged, this, [enforceLayer](Qt::WindowState state) {
            qDebug() << "[OverlayMaskHandler] windowStateChanged ->" << state << "; re-enforcing layer config.";
            enforceLayer();
        });
        connect(m_window, &QQuickWindow::visibleChanged, this, [enforceLayer, window]() {
            qDebug() << "[OverlayMaskHandler] visibleChanged (window isVisible =" << window->isVisible()
            << "); re-enforcing layer config.";
            enforceLayer();
        });
    } else {
        qWarning() << "[OverlayMaskHandler] LayerShellQt::Window::get() returned null! "
        "Is the platform plugin loaded correctly?";
    }

    // 2. Track geometry/visibility changes on overlayCard.
    qDebug() << "[OverlayMaskHandler] Searching for overlayCard child in window...";
    QObject *overlayCard = m_window->findChild<QObject*>("overlayCard");
    if (overlayCard) {
        qDebug() << "[OverlayMaskHandler] overlayCard found:" << overlayCard;

        connect(overlayCard, &QObject::destroyed, this, [this]() {
            qDebug() << "[OverlayMaskHandler] overlayCard was destroyed. Clearing window mask.";
            m_window->setMask(QRegion());
        });
        connect(overlayCard, SIGNAL(xChanged()),       m_window, SLOT(update()));
        connect(overlayCard, SIGNAL(yChanged()),       m_window, SLOT(update()));
        connect(overlayCard, SIGNAL(widthChanged()),   m_window, SLOT(update()));
        connect(overlayCard, SIGNAL(heightChanged()),  m_window, SLOT(update()));
        connect(overlayCard, SIGNAL(visibleChanged()), m_window, SLOT(update()));

        qDebug() << "[OverlayMaskHandler] Connected geometry/visibility signals from overlayCard -> window update().";
    } else {
        qWarning() << "[OverlayMaskHandler] overlayCard NOT found in window! "
        "Mask will not track the card's geometry dynamically.";
    }

    QObject *topBar = m_window->findChild<QObject*>("topBarItem");
    if (topBar) {
        connect(topBar, SIGNAL(visibleChanged()), m_window, SLOT(update()));
        qDebug() << "[OverlayMaskHandler] Connected visibility signals from topBar -> window update().";
    }
    else {
        qWarning() << "[OverlayMaskHandler] no topbar exists";
    }


    // 3. Keep updating mask across animation refresh cycles.
    connect(m_window, &QQuickWindow::afterAnimating, this, [this]() {
        updateOverlayMask(m_window);
    });
    qDebug() << "[OverlayMaskHandler] Connected afterAnimating -> updateOverlayMask().";

    qDebug() << "[OverlayMaskHandler] Constructor complete.";
}


void OverlayMaskHandler::updateOverlayMask(QQuickWindow *window)
{
    if (!window) return;

    QObject *overlayCard = window->findChild<QObject*>("overlayCard");
    QObject *topBar = window->findChild<QObject*>("topBarItem");
    QObject *crosshair = window->findChild<QObject*>("crosshairItem");

    QRegion combinedRegion;

    // 1. Add the Overlay Menu Card if it's visible
    if (overlayCard && overlayCard->property("visible").toBool()) {
        int x = overlayCard->property("x").toInt();
        int y = overlayCard->property("y").toInt();
        int w = overlayCard->property("width").toInt();
        int h = overlayCard->property("height").toInt();
        combinedRegion += QRegion(x, y, w, h);
    }

    // 2. Add the Top Bar if it's visible (Takes up full width at top of screen)
    if (topBar && topBar->property("visible").toBool()) {
        int h = topBar->property("height").toInt();
        if (h <= 0) h = 34; // Fallback to QML default height if not evaluated yet
        combinedRegion += QRegion(0, 0, window->width(), h);
    }

    // 3. Add the Crosshair region
    if (crosshair && crosshair->property("visible").toBool()) {
        int cx = (window->width() / 2) - 20;
        int cy = (window->height() / 2) - 20;
        combinedRegion += QRegion(cx, cy, 40, 40);
    }

    // Apply the combined layout mask to Wayland
    if (combinedRegion.isEmpty()) {
        window->setMask(QRegion(-1, -1, 1, 1));
    } else {
        window->setMask(combinedRegion);
    }
}
