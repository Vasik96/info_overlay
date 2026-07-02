/****************************************************************************
** Meta object code from reading C++ file 'PortalShortcuts.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/PortalShortcuts.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PortalShortcuts.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.11.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN15PortalShortcutsE_t {};
} // unnamed namespace

template <> constexpr inline auto PortalShortcuts::qt_create_metaobjectdata<qt_meta_tag_ZN15PortalShortcutsE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "PortalShortcuts",
        "shortcutLabelChanged",
        "",
        "displayModeChanged",
        "handleSessionCreatedResponse",
        "code",
        "QVariantMap",
        "results",
        "handleBindShortcutsResponse",
        "handleListShortcutsResponse",
        "handleActivated",
        "QDBusObjectPath",
        "session",
        "id",
        "timestamp",
        "options",
        "handleShortcutsChanged",
        "ShortcutList",
        "list",
        "shortcutLabel",
        "displayMode"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'shortcutLabelChanged'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'displayModeChanged'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'handleSessionCreatedResponse'
        QtMocHelpers::SlotData<void(uint, const QVariantMap &)>(4, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::UInt, 5 }, { 0x80000000 | 6, 7 },
        }}),
        // Slot 'handleBindShortcutsResponse'
        QtMocHelpers::SlotData<void(uint, const QVariantMap &)>(8, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::UInt, 5 }, { 0x80000000 | 6, 7 },
        }}),
        // Slot 'handleListShortcutsResponse'
        QtMocHelpers::SlotData<void(uint, const QVariantMap &)>(9, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::UInt, 5 }, { 0x80000000 | 6, 7 },
        }}),
        // Slot 'handleActivated'
        QtMocHelpers::SlotData<void(const QDBusObjectPath &, const QString &, qulonglong, const QVariantMap &)>(10, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 11, 12 }, { QMetaType::QString, 13 }, { QMetaType::ULongLong, 14 }, { 0x80000000 | 6, 15 },
        }}),
        // Slot 'handleShortcutsChanged'
        QtMocHelpers::SlotData<void(QDBusObjectPath, ShortcutList)>(16, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 11, 12 }, { 0x80000000 | 17, 18 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'shortcutLabel'
        QtMocHelpers::PropertyData<QString>(19, QMetaType::QString, QMC::DefaultPropertyFlags, 0),
        // property 'displayMode'
        QtMocHelpers::PropertyData<int>(20, QMetaType::Int, QMC::DefaultPropertyFlags, 1),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<PortalShortcuts, qt_meta_tag_ZN15PortalShortcutsE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject PortalShortcuts::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15PortalShortcutsE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15PortalShortcutsE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN15PortalShortcutsE_t>.metaTypes,
    nullptr
} };

void PortalShortcuts::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<PortalShortcuts *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->shortcutLabelChanged(); break;
        case 1: _t->displayModeChanged(); break;
        case 2: _t->handleSessionCreatedResponse((*reinterpret_cast<std::add_pointer_t<uint>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QVariantMap>>(_a[2]))); break;
        case 3: _t->handleBindShortcutsResponse((*reinterpret_cast<std::add_pointer_t<uint>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QVariantMap>>(_a[2]))); break;
        case 4: _t->handleListShortcutsResponse((*reinterpret_cast<std::add_pointer_t<uint>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QVariantMap>>(_a[2]))); break;
        case 5: _t->handleActivated((*reinterpret_cast<std::add_pointer_t<QDBusObjectPath>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<qulonglong>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QVariantMap>>(_a[4]))); break;
        case 6: _t->handleShortcutsChanged((*reinterpret_cast<std::add_pointer_t<QDBusObjectPath>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<ShortcutList>>(_a[2]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QDBusObjectPath >(); break;
            }
            break;
        case 6:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QDBusObjectPath >(); break;
            case 1:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< ShortcutList >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (PortalShortcuts::*)()>(_a, &PortalShortcuts::shortcutLabelChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (PortalShortcuts::*)()>(_a, &PortalShortcuts::displayModeChanged, 1))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<QString*>(_v) = _t->shortcutLabel(); break;
        case 1: *reinterpret_cast<int*>(_v) = _t->displayMode(); break;
        default: break;
        }
    }
}

const QMetaObject *PortalShortcuts::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PortalShortcuts::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15PortalShortcutsE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int PortalShortcuts::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void PortalShortcuts::shortcutLabelChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void PortalShortcuts::displayModeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
