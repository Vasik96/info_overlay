#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <LayerShellQt/Window>
#include <QRegion>
#include <QDBusConnection>
#include <QCoreApplication>
#include <QProcessEnvironment>
#include <QtWebEngineQuick>

#include "OverlayAdaptor.h"

LayerShellQt::Window* globalLayerWindow = nullptr;

void updateOverlayMask(QQuickWindow *window) {
    if (!window) return;

    QObject *overlayCard = window->findChild<QObject*>("overlayCard");
    if (!overlayCard || !overlayCard->property("visible").toBool()) {
        // FIX: Instead of QRegion(), pass a 1x1 pixel box completely off-screen.
        // This explicitly forces Wayland to free up the visible screen space.
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

int main(int argc, char *argv[])
{
    QtWebEngineQuick::initialize();

    qputenv("QT_QPA_PLATFORM", "wayland");

    QGuiApplication app(argc, argv);
    app.setApplicationName("info_overlay");
    app.setOrganizationDomain("local.project");
    app.setApplicationDisplayName("Info Overlay");

    QQmlApplicationEngine engine;
    engine.loadFromModule("info_overlay", "Main");
    if (engine.rootObjects().isEmpty()) return -1;

    QQuickWindow *window = qobject_cast<QQuickWindow*>(engine.rootObjects().first());
    if (window) {
        // 1. Layer Shell Configuration
        LayerShellQt::Window *layerWindow = LayerShellQt::Window::get(window);

        if (layerWindow) {

            globalLayerWindow = layerWindow;


            // Define a lambda to re-apply the layer
            auto enforceLayer = [layerWindow]() {
                layerWindow->setLayer(LayerShellQt::Window::LayerOverlay);
                layerWindow->setKeyboardInteractivity(LayerShellQt::Window::KeyboardInteractivityOnDemand);
            };

            enforceLayer(); // Initial set

            // CRITICAL: Re-enforce the layer whenever the window state changes
            // (e.g., when un-minimizing or returning from Meta+D)
            QObject::connect(window, &QQuickWindow::windowStateChanged, [enforceLayer]() {
                enforceLayer();
            });

            // Also re-enforce when the window is mapped/shown
            QObject::connect(window, &QQuickWindow::visibleChanged, [enforceLayer]() {
                enforceLayer();
            });
        }

        // 2. Click-through mask tracking
        QObject *overlayCard = window->findChild<QObject*>("overlayCard");
        if (overlayCard) {
            // Recalculate input bounding boxes when the CARD changes visibility states
            QObject::connect(overlayCard, &QObject::destroyed, window, [window]() { window->setMask(QRegion()); });

            // Re-sync properties to let slot recalculate positioning shifts
            QObject::connect(overlayCard, SIGNAL(xChanged()),      window, SLOT(update()));
            QObject::connect(overlayCard, SIGNAL(yChanged()),      window, SLOT(update()));
            QObject::connect(overlayCard, SIGNAL(widthChanged()),  window, SLOT(update()));
            QObject::connect(overlayCard, SIGNAL(heightChanged()), window, SLOT(update()));

            // Connect to property tracking updates for the Card's internal visibility shifts
            QObject::connect(overlayCard, SIGNAL(visibleChanged()), window, SLOT(update()));
        }

        // Use the default state rendering callback loops
        QObject::connect(window, &QQuickWindow::afterAnimating,
                         [window]() { updateOverlayMask(window); });


        // 3. DBus service — adaptor owns the Toggle/Show/Hide interface
        // The adaptor's parent is a plain QObject that we register at /Overlay
        QObject *serviceRoot = new QObject(&app);
        new OverlayAdaptor(serviceRoot, window);   // adaptor parented to serviceRoot

        QDBusConnection bus = QDBusConnection::sessionBus();
        bus.registerService("org.info_overlay.Overlay");
        bus.registerObject("/Overlay", serviceRoot);

        // 4. Show
        window->show();
    }

    return app.exec();
}
