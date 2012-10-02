/****************************************************************************
** Meta object code from reading C++ file 'knob.h'
**
** Created: Tue Oct 2 11:10:42 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "knob.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'knob.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Knob[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
       6,    5,    5,    5, 0x08,
      32,    5,    5,    5, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Knob[] = {
    "Knob\0\0on_dial_valueChanged(int)\0"
    "on_txtVal_editingFinished()\0"
};

const QMetaObject Knob::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Knob,
      qt_meta_data_Knob, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Knob::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Knob::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Knob::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Knob))
        return static_cast<void*>(const_cast< Knob*>(this));
    if (!strcmp(_clname, "Ui::knob"))
        return static_cast< Ui::knob*>(const_cast< Knob*>(this));
    return QWidget::qt_metacast(_clname);
}

int Knob::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: on_dial_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: on_txtVal_editingFinished(); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
