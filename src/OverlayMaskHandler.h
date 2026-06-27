#pragma once
#include <QObject>
#include <QQuickWindow>

class OverlayMaskHandler : public QObject {
    Q_OBJECT
public:
    explicit OverlayMaskHandler(QQuickWindow *window, QObject *parent = nullptr);

    static void updateOverlayMask(QQuickWindow *window);

private:
    QQuickWindow *m_window;
};
