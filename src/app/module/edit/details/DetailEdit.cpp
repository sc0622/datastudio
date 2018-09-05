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

    groupBox_ = new QGroupBox(tr("Details"), this);
    vertLayoutMain->addWidget(groupBox_);

    layoutEdit_ = new QVBoxLayout(groupBox_);
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

void DetailEdit::updateView(const Icd::ObjectPtr &object)
{
    resetView();

    object_ = object;

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
        const Icd::ItemPtr item = JHandlePtrCast<Icd::Item>(object);
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
        const Icd::RootPtr root = JHandlePtrCast<Icd::Root>(object);
        if (!root) {
            return;
        }
        object_ = root->vehicleAt(index.toInt());
        break;
    }
    case Icd::ObjectVehicle:
    {
        const Icd::VehiclePtr vehicle = JHandlePtrCast<Icd::Vehicle>(object);
        if (!vehicle) {
            return;
        }
        object_ = vehicle->systemAt(index.toInt());
        break;
    }
    case Icd::ObjectSystem:
    {
        const Icd::SystemPtr system = JHandlePtrCast<Icd::System>(object);
        if (!system) {
            return;
        }
        object_ = system->tableAt(index.toInt());
        break;
    }
    case Icd::ObjectTable:
    {
        const Icd::TablePtr table = JHandlePtrCast<Icd::Table>(object);
        if (!table) {
            return;
        }
        object_ = table->itemAt(index.toInt());
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
            object_ = frame->tableAt(index.toULongLong());
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

    if (!object_) {
        return;
    }

    objectEdit_ = ObjectEdit::create(object_);
    if (!objectEdit_) {
        return;
    }

    layoutEdit_->addWidget(objectEdit_);

    connect(objectEdit_, &ObjectEdit::contentChanged, this, &DetailEdit::contentChanged);

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
