/****************************************************************************
** Meta object code from reading C++ file 'sbaslercameracontrol.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../sbaslercameracontrol.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'sbaslercameracontrol.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_SBaslerCameraControl_t {
    QByteArrayData data[11];
    char stringdata0[120];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SBaslerCameraControl_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SBaslerCameraControl_t qt_meta_stringdata_SBaslerCameraControl = {
    {
QT_MOC_LITERAL(0, 0, 20), // "SBaslerCameraControl"
QT_MOC_LITERAL(1, 21, 15), // "sigCameraUpdate"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 4), // "list"
QT_MOC_LITERAL(4, 43, 13), // "sigSizeChange"
QT_MOC_LITERAL(5, 57, 4), // "size"
QT_MOC_LITERAL(6, 62, 14), // "sigCameraCount"
QT_MOC_LITERAL(7, 77, 5), // "count"
QT_MOC_LITERAL(8, 83, 15), // "sigCurrentImage"
QT_MOC_LITERAL(9, 99, 3), // "img"
QT_MOC_LITERAL(10, 103, 16) // "onTimerGrabImage"

    },
    "SBaslerCameraControl\0sigCameraUpdate\0"
    "\0list\0sigSizeChange\0size\0sigCameraCount\0"
    "count\0sigCurrentImage\0img\0onTimerGrabImage"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SBaslerCameraControl[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x06 /* Public */,
       4,    1,   42,    2, 0x06 /* Public */,
       6,    1,   45,    2, 0x06 /* Public */,
       8,    1,   48,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      10,    0,   51,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QStringList,    3,
    QMetaType::Void, QMetaType::QSize,    5,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void, QMetaType::QImage,    9,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void SBaslerCameraControl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SBaslerCameraControl *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sigCameraUpdate((*reinterpret_cast< QStringList(*)>(_a[1]))); break;
        case 1: _t->sigSizeChange((*reinterpret_cast< QSize(*)>(_a[1]))); break;
        case 2: _t->sigCameraCount((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->sigCurrentImage((*reinterpret_cast< QImage(*)>(_a[1]))); break;
        case 4: _t->onTimerGrabImage(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (SBaslerCameraControl::*)(QStringList );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SBaslerCameraControl::sigCameraUpdate)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (SBaslerCameraControl::*)(QSize );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SBaslerCameraControl::sigSizeChange)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (SBaslerCameraControl::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SBaslerCameraControl::sigCameraCount)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (SBaslerCameraControl::*)(QImage );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SBaslerCameraControl::sigCurrentImage)) {
                *result = 3;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject SBaslerCameraControl::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_SBaslerCameraControl.data,
    qt_meta_data_SBaslerCameraControl,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *SBaslerCameraControl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SBaslerCameraControl::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SBaslerCameraControl.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int SBaslerCameraControl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void SBaslerCameraControl::sigCameraUpdate(QStringList _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void SBaslerCameraControl::sigSizeChange(QSize _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void SBaslerCameraControl::sigCameraCount(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void SBaslerCameraControl::sigCurrentImage(QImage _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
