#ifndef DETAILEDIT_H
#define DETAILEDIT_H

#include <QWidget>
#include "main_global.h"
#include <memory>

class QGroupBox;
class QVBoxLayout;

namespace Icd {
class Object;
typedef std::shared_ptr<Object> ObjectPtr;
class Vehicle;
typedef std::shared_ptr<Vehicle> VehiclePtr;
class System;
typedef std::shared_ptr<System> SystemPtr;
class Table;
typedef std::shared_ptr<Table> TablePtr;
class Item;
typedef std::shared_ptr<Item> ItemPtr;
class HeaderItem;
typedef std::shared_ptr<HeaderItem> HeaderItemPtr;
class CounterItem;
typedef std::shared_ptr<CounterItem> CounterItemPtr;
class CheckItem;
typedef std::shared_ptr<CheckItem> CheckItemPtr;
class FrameCodeItem;
typedef std::shared_ptr<FrameCodeItem> FrameCodeItemPtr;
class NumericItem;
typedef std::shared_ptr<NumericItem> NumericItemPtr;
class ArrayItem;
typedef std::shared_ptr<ArrayItem> ArrayItemPtr;
class BitItem;
typedef std::shared_ptr<BitItem> BitItemPtr;
class ComplexItem;
typedef std::shared_ptr<ComplexItem> ComplexItemPtr;
class FrameItem;
typedef std::shared_ptr<FrameItem> FrameItemPtr;
}

class QPushButton;

namespace Edit {

class ObjectEdit;

class DetailEdit : public QWidget
{
    Q_OBJECT
public:
    explicit DetailEdit(QWidget *parent = nullptr);

    void resetView();
    void updateView(const Icd::ObjectPtr &object, const QVariant &index);
    void updateView(const Icd::ObjectPtr &object, bool sub = false, bool add = false);
    void setButtonsEnabled(bool enabled);

    Icd::ObjectPtr target() const;

signals:
    void applied();
    void canceled();

public slots:
    void onContentChanged(const QString &key, const QVariant &value);
    void restoreContent();

private:
    void removeEdit();
    void changeEdit(int itemType);

private:
    QVBoxLayout *layoutEdit_;
    ObjectEdit *objectEdit_;
    QPushButton *buttonApply_;
    QPushButton *buttonCancel_;
    Icd::ObjectPtr object_;
    Icd::ObjectPtr newObject_;
};

}

#endif // DETAILEDIT_H
