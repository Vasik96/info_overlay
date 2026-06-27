#include "OverlayMaskHandler.h"
#include <LayerShellQt/Window>
#include <QRegion>

OverlayMaskHandler::OverlayMaskHandler(QQuickWindow *window, QObject *parent)
: QObject(parent), m_window(window)
{
    if (!m_window) return;

    // 1. Setup LayerShell configurations securely
    LayerShellQt::Window *layerWindow = LayerShellQt::Window::get(m_window);
    if (layerWindow) {
        auto enforceLayer = [layerWindow]() {
            layerWindow->setLayer(LayerShellQt::Window::LayerOverlay);
            layerWindow->setKeyboardInteractivity(LayerShellQt::Window::KeyboardInteractivityOnDemand);
        };

        enforceLayer();
        connect(m_window, &QQuickWindow::windowStateChanged, this, enforceLayer);
        connect(m_window, &QQuickWindow::visibleChanged, this, enforceLayer);
    }

    // 2. Track specific changes from our item card child components
    QObject *overlayCard = m_window->findChild<QObject*>("overlayCard");
    if (overlayCard) {
        connect(overlayCard, &QObject::destroyed, this, [this]() { m_window->setMask(QRegion()); });
        connect(overlayCard, SIGNAL(xChanged()),      m_window, SLOT(update()));
        connect(overlayCard, SIGNAL(yChanged()),      m_window, SLOT(update()));
        connect(overlayCard, SIGNAL(widthChanged()),  m_window, SLOT(update()));
        connect(overlayCard, SIGNAL(heightChanged()), m_window, SLOT(update()));
        connect(overlayCard, SIGNAL(visibleChanged()), m_window, SLOT(update()));
    }

    // 3. Keep updating geometry positions across animation refresh cycles
    connect(m_window, &QQuickWindow::afterAnimating, this, [this]() {
        updateOverlayMask(m_window);
    });
}

void OverlayMaskHandler::updateOverlayMask(QQuickWindow *window) {
    if (!window) return;

    QObject *overlayCard = window->findChild<QObject*>("overlayCard");
    if (!overlayCard || !overlayCard->property("visible").toBool()) {
        window->setMask(QRegion(-1, -1, 1, 1));
        return;
    }

    window->setMask(QRegion(
        overlayCard->property("x").toInt(),
                            overlayCard->property("y").toInt(),
                            overlayCard->property("width").toInt(),
                            overlayCard->property("height").toInt()
    ));
}
