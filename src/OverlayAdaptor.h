#pragma once
#include <QDBusAbstractAdaptor>
#include <QQuickWindow>
#include <QQuickItem>
#include <QDebug>

class OverlayAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.info_overlay.Overlay")

public:
    explicit OverlayAdaptor(QObject *parent, QQuickWindow *window)
    : QDBusAbstractAdaptor(parent), m_window(window) {}

public slots:
    void Toggle() {
        QObject *card = getCard();
        if (!card) return;

        bool nowVisible = !card->property("visible").toBool();
        card->setProperty("visible", nowVisible);
        updateMask(nowVisible);
    }

    void Show() {
        QObject *card = getCard();
        if (!card) return;

        card->setProperty("visible", true);
        updateMask(true);
    }

    void Hide() {
        QObject *card = getCard();
        if (!card) return;

        card->setProperty("visible", false);
        updateMask(false);
    }

private:
    QObject *getCard() {
        if (!m_window) return nullptr;
        QObject *card = m_window->findChild<QObject*>("overlayCard");
        if (!card) qWarning() << "[DBus] overlayCard is null";
        return card;
    }

    void updateMask(bool visible) {
        if (!m_window) return;
        if (!visible) {
            m_window->setMask(QRegion());
            return;
        }
        QObject *card = getCard();
        if (card) {
            m_window->setMask(QRegion(
                card->property("x").toInt(),
                card->property("y").toInt(),
                card->property("width").toInt(),
                card->property("height").toInt()
            ));
        }
    }

    QQuickWindow *m_window;
};
