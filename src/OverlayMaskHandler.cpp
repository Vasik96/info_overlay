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
            qDebug() << "[OverlayMaskHandler] enforceLayer() called — setting LayerOverlay + KeyboardInteractivityOnDemand.";
            layerWindow->setLayer(LayerShellQt::Window::LayerOverlay);
            layerWindow->setKeyboardInteractivity(LayerShellQt::Window::KeyboardInteractivityOnDemand);
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

    // 3. Keep updating mask across animation refresh cycles.
    connect(m_window, &QQuickWindow::afterAnimating, this, [this]() {
        updateOverlayMask(m_window);
    });
    qDebug() << "[OverlayMaskHandler] Connected afterAnimating -> updateOverlayMask().";

    qDebug() << "[OverlayMaskHandler] Constructor complete.";
}


void OverlayMaskHandler::updateOverlayMask(QQuickWindow *window)
{
    if (!window) {
        qWarning() << "[OverlayMaskHandler] updateOverlayMask() called with null window, skipping.";
        return;
    }

    QObject *overlayCard = window->findChild<QObject*>("overlayCard");

    if (!overlayCard) {
        qWarning() << "[OverlayMaskHandler] updateOverlayMask(): overlayCard not found in window. "
        "Applying minimal 1x1 mask.";
        window->setMask(QRegion(-1, -1, 1, 1));
        return;
    }

    const bool visible = overlayCard->property("visible").toBool();
    if (!visible) {
        qDebug() << "[OverlayMaskHandler] overlayCard is hidden. Applying minimal 1x1 mask.";
        window->setMask(QRegion(-1, -1, 1, 1));
        return;
    }

    const int x = overlayCard->property("x").toInt();
    const int y = overlayCard->property("y").toInt();
    const int w = overlayCard->property("width").toInt();
    const int h = overlayCard->property("height").toInt();

    qDebug() << "[OverlayMaskHandler] updateOverlayMask(): overlayCard visible at"
    << x << y << w << "x" << h << "— applying mask.";

    window->setMask(QRegion(x, y, w, h));
}
