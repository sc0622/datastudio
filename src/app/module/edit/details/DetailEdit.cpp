#include "precomp.h"
#include "DetailEdit.h"
#include "../itemedit/ObjectEdit.h"

namespace Edit {

DetailEdit::DetailEdit(QWidget *parent)
    : QWidget(parent)
    , objectEdit_(nullptr)
    , object_(nullptr)
{
    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);
    vertLayoutMain->setSpacing(0);

    layoutEdit_ = new QVBoxLayout();
    layoutEdit_->setContentsMargins(0, 0, 0, 0);
    layoutEdit_->setSpacing(3);
    vertLayoutMain->addLayout(layoutEdit_);
}

void DetailEdit::resetView()
{
    if (objectEdit_) {
        layoutEdit_->removeWidget(objectEdit_);
        objectEdit_->disconnect();
        objectEdit_->deleteLater();
        objectEdit_ = nullptr;
    }

    object_ = nullptr;
    hide();
}

void DetailEdit::updateView(const Icd::ObjectPtr &object, const QVariant &index)
{
    resetView();

    if (!object || index.type() != QVariant::ULongLong && index.toInt() == -1) {
        return;
    }

    Icd::ObjectPtr subObject;

    switch (object->objectType()) {
    case Icd::ObjectRoot:
    {
        const Icd::RootPtr root = JHandlePtrCast<Icd::Root>(object);
        if (!root) {
            return;
        }
        subObject = root->vehicleAt(index.toInt());
        break;
    }
    case Icd::ObjectVehicle:
    {
        const Icd::VehiclePtr vehicle = JHandlePtrCast<Icd::Vehicle>(object);
        if (!vehicle) {
            return;
        }
        subObject = vehicle->systemAt(index.toInt());
        break;
    }
    case Icd::ObjectSystem:
    {
        const Icd::SystemPtr system = JHandlePtrCast<Icd::System>(object);
        if (!system) {
            return;
        }
        subObject = system->tableAt(index.toInt());
        break;
    }
    case Icd::ObjectTable:
    {
        const Icd::TablePtr table = JHandlePtrCast<Icd::Table>(object);
        if (!table) {
            return;
        }
        subObject = table->itemAt(index.toInt());
        break;
    }
    case Icd::ObjectItem:
    {
        const Icd::ItemPtr item = JHandlePtrCast<Icd::Item>(object);
        if (!item) {
            break;
        }
        switch (item->type()) {
        case Icd::ItemFrame:
        {
            const Icd::FrameItemPtr frame = JHandlePtrCast<Icd::FrameItem>(object);
            if (!frame) {
                return;
            }
            subObject = frame->tableAt(index.toULongLong());
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

    if (!subObject) {
        return;
    }

    updateView(subObject, true);
}

void DetailEdit::updateView(const Icd::ObjectPtr &object, bool sub)
{
    if (!sub) {
        resetView();
    }

    object_ = object;

    if (!object_) {
        return;
    }

    objectEdit_ = ObjectEdit::create(object_);
    if (!objectEdit_) {
        return;
    }

    layoutEdit_->addWidget(objectEdit_);

    connect(objectEdit_, &ObjectEdit::contentChanged, this, &DetailEdit::contentChanged);

    if (!objectEdit_->init()) {
        //
    }

    show();
}

void DetailEdit::updateContent(const QString &name)
{
    if (!objectEdit_ || isHidden()) {
        return;
    }

    return objectEdit_->updateContent(name);
}

}
