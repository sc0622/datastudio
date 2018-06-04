#include "precomp.h"
#include "DetailEdit.h"

namespace Edit {

DetailEdit::DetailEdit(QWidget *parent)
    : QWidget(parent)
    , d_objectEdit(nullptr)
    , d_object(nullptr)
{
    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);
    vertLayoutMain->setSpacing(0);

    d_groupBox = new QGroupBox(tr("Details"), this);
    vertLayoutMain->addWidget(d_groupBox);

    d_layoutEdit = new QVBoxLayout(d_groupBox);
}

void DetailEdit::resetView()
{
    if (d_objectEdit) {
        d_layoutEdit->removeWidget(d_objectEdit);
        d_objectEdit->disconnect();
        d_objectEdit->deleteLater();
        d_objectEdit = nullptr;
    }

    d_object = nullptr;
    hide();
}

void DetailEdit::updateView(const Icd::ObjectPtr &object)
{
    resetView();

    d_object = object;

    if (!object) {
        return;
    }

    switch (object->objectType()) {
    case Icd::ObjectRoot:
    case Icd::ObjectVehicle:
    case Icd::ObjectSystem:
    case Icd::ObjectTable:
        break;
    case Icd::ObjectItem:
    {
        const Icd::ItemPtr item = JHandlePtrCast<Icd::Item, Icd::Object>(object);
        if (!item) {
            break;
        }
        switch (item->type()) {
        case Icd::ItemFrame:
            break;
        default:
            show();
            break;
        }
        break;
    }
    default:
        break;
    }
}

void DetailEdit::updateView(const Icd::ObjectPtr &object, const QVariant &index)
{
    resetView();

    if (!object || index.type() != QVariant::ULongLong && index.toInt() == -1) {
        return;
    }

    switch (object->objectType()) {
    case Icd::ObjectRoot:
    {
        const Icd::RootPtr root = JHandlePtrCast<Icd::Root, Icd::Object>(object);
        if (!root) {
            return;
        }
        d_object = root->vehicleAt(index.toInt());
        break;
    }
    case Icd::ObjectVehicle:
    {
        const Icd::VehiclePtr vehicle = JHandlePtrCast<Icd::Vehicle, Icd::Object>(object);
        if (!vehicle) {
            return;
        }
        d_object = vehicle->systemAt(index.toInt());
        break;
    }
    case Icd::ObjectSystem:
    {
        const Icd::SystemPtr system = JHandlePtrCast<Icd::System, Icd::Object>(object);
        if (!system) {
            return;
        }
        d_object = system->tableAt(index.toInt());
        break;
    }
    case Icd::ObjectTable:
    {
        const Icd::TablePtr table = JHandlePtrCast<Icd::Table, Icd::Object>(object);
        if (!table) {
            return;
        }
        d_object = table->itemAt(index.toInt());
        break;
    }
    case Icd::ObjectItem:
    {
        const Icd::ItemPtr item = JHandlePtrCast<Icd::Item, Icd::Object>(object);
        if (!item) {
            break;
        }
        switch (item->type()) {
        case Icd::ItemFrame:
        {
            const Icd::FrameItemPtr frame = JHandlePtrCast<Icd::FrameItem, Icd::Object>(object);
            if (!frame) {
                return;
            }
            d_object = frame->tableAt(index.toULongLong());
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }

    if (!d_object) {
        return;
    }

    d_objectEdit = ObjectEdit::create(d_object);
    if (!d_objectEdit) {
        return;
    }

    d_layoutEdit->addWidget(d_objectEdit);

    connect(d_objectEdit, &ObjectEdit::contentChanged, this, &DetailEdit::contentChanged);

    show();
}

void DetailEdit::updateContent(const QString &name)
{
    if (!d_objectEdit || isHidden()) {
        return;
    }

    return d_objectEdit->updateContent(name);
}

//// - private

// class ObjectEdit

ObjectEdit::ObjectEdit(const Icd::ObjectPtr &object, QWidget *parent)
    : QWidget(parent)
    , d_object(object)
{
    QHBoxLayout *horiLayoutMain = new QHBoxLayout(this);
    horiLayoutMain->setContentsMargins(0, 0, 0, 0);

    QFormLayout *formLayoutLeft = new QFormLayout();
    formLayoutLeft->setLabelAlignment(Qt::AlignRight);
    horiLayoutMain->addLayout(formLayoutLeft);

    d_editName = new QLineEdit(this);
    formLayoutLeft->addRow(tr("Name:"), d_editName);

    d_editMark = new QLineEdit(this);
    formLayoutLeft->addRow(tr("Mark:"), d_editMark);

    d_editDesc = new QPlainTextEdit(this);
    formLayoutLeft->addRow(tr("Describe:"), d_editDesc);

    if (object) {
        //
        d_editName->setText(QString::fromStdString(object->name()));
        d_editMark->setText(QString::fromStdString(object->mark()));
        d_editDesc->setPlainText(QString::fromStdString(object->desc()));
        //
        connect(d_editName, &QLineEdit::textChanged, this, [=](const QString &text){
            if (d_object) {
                d_object->setName(text.toStdString());
                emit contentChanged("name");
            }
        });
        connect(d_editMark, &QLineEdit::textChanged, this, [=](const QString &text){
            if (d_object) {
                d_object->setMark(text.toStdString());
                emit contentChanged("mark");
            }
        });
        connect(d_editDesc, &QPlainTextEdit::textChanged, this, [=](){
            if (d_object) {
                d_object->setDesc(d_editDesc->toPlainText().toStdString());
                emit contentChanged("desc");
            }
        });
    }
}

ObjectEdit::~ObjectEdit()
{

}

ObjectEdit *ObjectEdit::create(const Icd::ObjectPtr &object)
{
    if (!object) {
        return nullptr;
    }

    switch (object->objectType()) {
    case Icd::ObjectVehicle: return new VehicleEdit(object);
    case Icd::ObjectSystem: return new SystemEdit(object);
    case Icd::ObjectTable: return new TableEdit(object);
    case Icd::ObjectItem:
    {
        const Icd::ItemPtr item = JHandlePtrCast<Icd::Item, Icd::Object>(object);
        if (!item) {
            break;
        }
        switch (item->type()) {
        case Icd::ItemHead: return new HeaderEdit(object);
        case Icd::ItemCounter: return new CounterEdit(object);
        case Icd::ItemCheck: return new CheckEdit(object);
        case Icd::ItemFrameCode: return new FrameCodeEdit(object);
        case Icd::ItemNumeric: return new NumericEdit(object);
        case Icd::ItemBitMap: return new BitMapEdit(object);
        case Icd::ItemBitValue: return new BitValueEdit(object);
        case Icd::ItemComplex: return new ComplexEdit(object);
        case Icd::ItemFrame: return new FrameEdit(object);
        default:
            break;
        }
        break;
    }
    default:
        break;
    }

    return nullptr;
}

Icd::ObjectPtr ObjectEdit::object() const
{
    return d_object;
}

void ObjectEdit::updateContent(const QString &name)
{
    if (!d_object) {
        return;
    }

    if (name == "name") {
        d_editName->setText(QString::fromStdString(d_object->name()));
    } else if (name == "mark") {
        d_editMark->setText(QString::fromStdString(d_object->mark()));
    } else if (name == "desc") {
        d_editDesc->setPlainText(QString::fromStdString(d_object->desc()));
    }
}

// class VehicleEdit

VehicleEdit::VehicleEdit(const Icd::ObjectPtr &object, QWidget *parent)
    : ObjectEdit(object, parent)
{

}

VehicleEdit::~VehicleEdit()
{

}

Icd::VehiclePtr VehicleEdit::vehicle() const
{
    return JHandlePtrCast<Icd::Vehicle, Icd::Object>(object());
}

// class SystemEdit

SystemEdit::SystemEdit(const Icd::ObjectPtr &object, QWidget *parent)
    : ObjectEdit(object, parent)
{

}

SystemEdit::~SystemEdit()
{

}

Icd::SystemPtr SystemEdit::system() const
{
    return JHandlePtrCast<Icd::System, Icd::Object>(object());
}

// class TableEdit

TableEdit::TableEdit(const Icd::ObjectPtr &object, QWidget *parent)
    : ObjectEdit(object, parent)
{

}

TableEdit::~TableEdit()
{

}

Icd::TablePtr TableEdit::table() const
{
    return JHandlePtrCast<Icd::Table, Icd::Object>(object());
}

// class ItemEdit

ItemEdit::ItemEdit(const Icd::ObjectPtr &object, QWidget *parent)
    : ObjectEdit(object, parent)
{

}

ItemEdit::~ItemEdit()
{

}

Icd::ItemPtr ItemEdit::item() const
{
    return JHandlePtrCast<Icd::Item, Icd::Object>(object());
}

// class HeaderEdit

HeaderEdit::HeaderEdit(const Icd::ObjectPtr &object, QWidget *parent)
    : ItemEdit(object, parent)
{

}

HeaderEdit::~HeaderEdit()
{

}

Icd::HeaderItemPtr HeaderEdit::headerItem() const
{
    return JHandlePtrCast<Icd::HeaderItem, Icd::Object>(object());
}

// class CounterEdit

CounterEdit::CounterEdit(const Icd::ObjectPtr &object, QWidget *parent)
    : ItemEdit(object, parent)
{

}

CounterEdit::~CounterEdit()
{

}

Icd::CounterItemPtr CounterEdit::counterItem() const
{
    return JHandlePtrCast<Icd::CounterItem, Icd::Object>(object());
}

// class CheckEdit

CheckEdit::CheckEdit(const Icd::ObjectPtr &object, QWidget *parent)
    : ItemEdit(object, parent)
{

}

CheckEdit::~CheckEdit()
{

}

Icd::CheckItemPtr CheckEdit::checkItem() const
{
    return JHandlePtrCast<Icd::CheckItem, Icd::Object>(object());
}

// class FrameCodeEdit

FrameCodeEdit::FrameCodeEdit(const Icd::ObjectPtr &object, QWidget *parent)
    : ItemEdit(object, parent)
{

}

FrameCodeEdit::~FrameCodeEdit()
{

}

Icd::FrameCodeItemPtr FrameCodeEdit::frameCodeItem() const
{
    return JHandlePtrCast<Icd::FrameCodeItem, Icd::Object>(object());
}

// class NumericEdit

NumericEdit::NumericEdit(const Icd::ObjectPtr &object, QWidget *parent)
    : ItemEdit(object, parent)
{

}

NumericEdit::~NumericEdit()
{

}

Icd::NumericItemPtr NumericEdit::numericItem() const
{
    return JHandlePtrCast<Icd::NumericItem, Icd::Object>(object());
}

// class BitEdit

BitEdit::BitEdit(const Icd::ObjectPtr &object, QWidget *parent)
    : ItemEdit(object, parent)
{

}

BitEdit::~BitEdit()
{

}

Icd::BitItemPtr BitEdit::bitItem() const
{
    return JHandlePtrCast<Icd::BitItem, Icd::Object>(object());
}

// class BitMapEdit

BitMapEdit::BitMapEdit(const Icd::ObjectPtr &object, QWidget *parent)
    : BitEdit(object, parent)
{

}

BitMapEdit::~BitMapEdit()
{

}

// class BitValueEdit

BitValueEdit::BitValueEdit(const Icd::ObjectPtr &object, QWidget *parent)
    : BitEdit(object, parent)
{

}

BitValueEdit::~BitValueEdit()
{

}

// class ComplexEdit

ComplexEdit::ComplexEdit(const Icd::ObjectPtr &object, QWidget *parent)
    : ItemEdit(object, parent)
{

}

ComplexEdit::~ComplexEdit()
{

}

Icd::ComplexItemPtr ComplexEdit::complexItem() const
{
    return JHandlePtrCast<Icd::ComplexItem, Icd::Object>(object());
}

// class FrameEdit

FrameEdit::FrameEdit(const Icd::ObjectPtr &object, QWidget *parent)
    : ItemEdit(object, parent)
{

}

FrameEdit::~FrameEdit()
{

}

Icd::FrameItemPtr FrameEdit::frameItem() const
{
    return JHandlePtrCast<Icd::FrameItem, Icd::Object>(object());
}

}
