#ifndef DETAILEDIT_H
#define DETAILEDIT_H

#include <QWidget>
#include "main_global.h"

class QGroupBox;
class QVBoxLayout;

namespace Icd {
class Object;
typedef JHandlePtr<Object> ObjectPtr;

class Vehicle;
typedef JHandlePtr<Vehicle> VehiclePtr;

class System;
typedef JHandlePtr<System> SystemPtr;

class Table;
typedef JHandlePtr<Table> TablePtr;

class Item;
typedef JHandlePtr<Item> ItemPtr;

class HeaderItem;
typedef JHandlePtr<HeaderItem> HeaderItemPtr;

class CounterItem;
typedef JHandlePtr<CounterItem> CounterItemPtr;

class CheckItem;
typedef JHandlePtr<CheckItem> CheckItemPtr;

class FrameCodeItem;
typedef JHandlePtr<FrameCodeItem> FrameCodeItemPtr;

class NumericItem;
typedef JHandlePtr<NumericItem> NumericItemPtr;

class BitItem;
typedef JHandlePtr<BitItem> BitItemPtr;

class ComplexItem;
typedef JHandlePtr<ComplexItem> ComplexItemPtr;

class FrameItem;
typedef JHandlePtr<FrameItem> FrameItemPtr;
}

namespace Edit {

class ObjectEdit;

class DetailEdit : public QWidget
{
    Q_OBJECT
public:
    explicit DetailEdit(QWidget *parent = nullptr);

    void resetView();
    void updateView(const Icd::ObjectPtr &object);
    void updateView(const Icd::ObjectPtr &object, const QVariant &index);

signals:
    void contentChanged(const QString &name);

public slots:
    void updateContent(const QString &name);

private:
    QGroupBox *d_groupBox;
    QVBoxLayout *d_layoutEdit;
    ObjectEdit *d_objectEdit;
    Icd::ObjectPtr d_object;
};

//// - private

// class ObjectEdit

class ::QLineEdit;
class ::QPlainTextEdit;

class ObjectEdit : public QWidget
{
    Q_OBJECT
public:
    explicit ObjectEdit(const Icd::ObjectPtr &object, QWidget *parent = nullptr);
    virtual ~ObjectEdit();

    static ObjectEdit *create(const Icd::ObjectPtr &object);

    Icd::ObjectPtr object() const;

signals:
    void contentChanged(const QString &name);

public slots:
    virtual void updateContent(const QString &name);

private:
    Icd::ObjectPtr d_object;
    QLineEdit *d_editName;
    QLineEdit *d_editMark;
    QPlainTextEdit *d_editDesc;
};

// class VehicleEdit

class VehicleEdit : public ObjectEdit
{
    Q_OBJECT
public:
    explicit VehicleEdit(const Icd::ObjectPtr &object, QWidget *parent = nullptr);
    ~VehicleEdit();

    Icd::VehiclePtr vehicle() const;

signals:

public slots:

private:
};

// class SystemEdit

class SystemEdit : public ObjectEdit
{
    Q_OBJECT
public:
    explicit SystemEdit(const Icd::ObjectPtr &object, QWidget *parent = nullptr);
    ~SystemEdit();

    Icd::SystemPtr system() const;

signals:

public slots:

private:
};

// class TableEdit

class TableEdit : public ObjectEdit
{
    Q_OBJECT
public:
    explicit TableEdit(const Icd::ObjectPtr &object, QWidget *parent = nullptr);
    ~TableEdit();

    Icd::TablePtr table() const;

signals:

public slots:

private:
};

// class ItemEdit

class ItemEdit : public ObjectEdit
{
    Q_OBJECT
public:
    explicit ItemEdit(const Icd::ObjectPtr &object, QWidget *parent = nullptr);
    virtual ~ItemEdit();

    Icd::ItemPtr item() const;

signals:

public slots:

private:
};

// class HeaderEdit

class HeaderEdit : public ItemEdit
{
    Q_OBJECT
public:
    explicit HeaderEdit(const Icd::ObjectPtr &object, QWidget *parent = nullptr);
    ~HeaderEdit();

    Icd::HeaderItemPtr headerItem() const;

signals:

public slots:

private:
};

// class CounterEdit

class CounterEdit : public ItemEdit
{
    Q_OBJECT
public:
    explicit CounterEdit(const Icd::ObjectPtr &object, QWidget *parent = nullptr);
    ~CounterEdit();

    Icd::CounterItemPtr counterItem() const;

signals:

public slots:

private:
};

// class CheckEdit

class CheckEdit : public ItemEdit
{
    Q_OBJECT
public:
    explicit CheckEdit(const Icd::ObjectPtr &object, QWidget *parent = nullptr);
    ~CheckEdit();

    Icd::CheckItemPtr checkItem() const;

signals:

public slots:

private:
};

// class FrameCodeEdit

class FrameCodeEdit : public ItemEdit
{
    Q_OBJECT
public:
    explicit FrameCodeEdit(const Icd::ObjectPtr &object, QWidget *parent = nullptr);
    ~FrameCodeEdit();

    Icd::FrameCodeItemPtr frameCodeItem() const;

signals:

public slots:

private:

};

// class NumericEdit

class NumericEdit : public ItemEdit
{
    Q_OBJECT
public:
    explicit NumericEdit(const Icd::ObjectPtr &object, QWidget *parent = nullptr);
    ~NumericEdit();

    Icd::NumericItemPtr numericItem() const;

signals:

public slots:

private:
};

// class BitEdit

class BitEdit : public ItemEdit
{
    Q_OBJECT
public:
    explicit BitEdit(const Icd::ObjectPtr &object, QWidget *parent = nullptr);
    virtual ~BitEdit();

    Icd::BitItemPtr bitItem() const;

signals:

public slots:

private:
};

// class BitMapEdit

class BitMapEdit : public BitEdit
{
    Q_OBJECT
public:
    explicit BitMapEdit(const Icd::ObjectPtr &object, QWidget *parent = nullptr);
    ~BitMapEdit();

signals:

public slots:

private:
};

// class BitValueEdit

class BitValueEdit : public BitEdit
{
    Q_OBJECT
public:
    explicit BitValueEdit(const Icd::ObjectPtr &object, QWidget *parent = nullptr);
    ~BitValueEdit();

signals:

public slots:

private:
};

// class ComplexEdit

class ComplexEdit : public ItemEdit
{
    Q_OBJECT
public:
    explicit ComplexEdit(const Icd::ObjectPtr &object, QWidget *parent = nullptr);
    ~ComplexEdit();

    Icd::ComplexItemPtr complexItem() const;

signals:

public slots:

private:
};

// class FrameEdit

class FrameEdit : public ItemEdit
{
    Q_OBJECT
public:
    explicit FrameEdit(const Icd::ObjectPtr &object, QWidget *parent = nullptr);
    ~FrameEdit();

    Icd::FrameItemPtr frameItem() const;

signals:

public slots:

private:
};

}

#endif // DETAILEDIT_H
