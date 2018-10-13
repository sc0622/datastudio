#include "precomp.h"
#include "DetailEdit.h"
#include "../itemedit/ObjectEdit.h"

namespace Edit {

DetailEdit::DetailEdit(QWidget *parent)
    : QWidget(parent)
    , objectEdit_(nullptr)
{
    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);
    vertLayoutMain->setSpacing(0);

    layoutEdit_ = new QVBoxLayout();
    layoutEdit_->setContentsMargins(0, 0, 0, 0);
    layoutEdit_->setSpacing(3);
    vertLayoutMain->addLayout(layoutEdit_);

    QHBoxLayout *layoutButton = new QHBoxLayout();
    layoutButton->setContentsMargins(2, 2, 2, 2);
    vertLayoutMain->addLayout(layoutButton);

    layoutButton->addStretch();

    buttonApply_ = new QPushButton(QIcon(":/datastudio/image/global/apply.png"),
                                   tr("Apply"), this);
    buttonApply_->setMinimumWidth(100);
    buttonApply_->setEnabled(false);
    layoutButton->addWidget(buttonApply_);

    layoutButton->addStretch();

    buttonCancel_ = new QPushButton(QIcon(":/datastudio/image/global/cancel.png"),
                                    tr("Cancel"), this);
    buttonCancel_->setMinimumWidth(100);
    buttonCancel_->setEnabled(false);
    layoutButton->addWidget(buttonCancel_);

    layoutButton->addStretch();

    connect(buttonApply_, &QPushButton::clicked, this, [=](){
        if (!object_ || !objectEdit_ || isHidden()) {
            return;
        }
        if (!objectEdit_->trySaveContent()) {
            return;
        }
        setButtonsEnabled(false);
        //
        if (object_->isSimpleItem()) {
            object_ = objectEdit_->object();
        }
        //
        emit applied();
        //
        newObject_.reset();
    });
    connect(buttonCancel_, &QPushButton::clicked, this, [=](){
        if (!object_ || !objectEdit_ || isHidden()) {
            return;
        }
        if (newObject_) {
            resetView();
        } else {
            restoreContent();
        }
        //
        emit canceled();
    });
}

void DetailEdit::resetView()
{
    removeEdit();
    object_.reset();
    newObject_.reset();
    hide();
}

void DetailEdit::updateView(const Icd::ObjectPtr &object, const QVariant &index)
{
    resetView();

    if (!object) {
        return;
    }

    if ((index.type() != QVariant::ULongLong && index.toInt() == -1)) {
        updateView(object, false, false);
        return;
    }

    Icd::ObjectPtr subObject;

    switch (object->rtti()) {
    case Icd::ObjectRoot:
    {
        auto root = JHandlePtrCast<Icd::Root>(object);
        if (!root) {
            return;
        }
        subObject = root->vehicleAt(index.toInt());
        break;
    }
    case Icd::ObjectVehicle:
    {
        auto vehicle = JHandlePtrCast<Icd::Vehicle>(object);
        if (!vehicle) {
            return;
        }
        subObject = vehicle->systemAt(index.toInt());
        break;
    }
    case Icd::ObjectSystem:
    {
        auto system = JHandlePtrCast<Icd::System>(object);
        if (!system) {
            return;
        }
        subObject = system->tableAt(index.toInt());
        break;
    }
    case Icd::ObjectTable:
    {
        auto table = JHandlePtrCast<Icd::Table>(object);
        if (!table) {
            return;
        }
        subObject = table->itemAt(index.toInt());
        break;
    }
    case Icd::ObjectComplex:
    {
        auto complex = JHandlePtrCast<Icd::ComplexItem>(object);
        if (!complex) {
            return;
        }
        auto table = complex->table();
        if (!table) {
            return;
        }
        subObject = table->itemAt(index.toInt());
        break;
    }
    case Icd::ObjectFrame:
    {
        auto frame = JHandlePtrCast<Icd::FrameItem>(object);
        if (!frame) {
            return;
        }
        subObject = frame->tableAt(index.toULongLong());
        break;
    }
    default:
        break;
    }

    if (!subObject) {
        return;
    }

    updateView(subObject, true, false);
}

void DetailEdit::updateView(const Icd::ObjectPtr &object, bool sub, bool add)
{
    if (sub) {
        removeEdit();
    } else {
        resetView();
    }

    object_ = object;

    if (!object_) {
        return;
    }

    if (add) {
        newObject_ = object_;
        setButtonsEnabled(true);
    } else {
        newObject_ = nullptr;
    }

    objectEdit_ = ObjectEdit::create(object_);
    if (!objectEdit_) {
        return;
    }

    layoutEdit_->addWidget(objectEdit_);

    connect(objectEdit_, &ObjectEdit::contentChanged, this, &DetailEdit::onContentChanged);
    if (object_->objectType() == Icd::ObjectItem) {
        connect(objectEdit_, &ObjectEdit::itemTypeChanged, this, &DetailEdit::changeEdit);
    }

    if (!objectEdit_->init()) {
        Q_ASSERT(false);
        //return;
    }

    if (!sub) {
        objectEdit_->focusName();
    }

    show();
}

void DetailEdit::setButtonsEnabled(bool enabled)
{
    buttonApply_->setEnabled(enabled);
    buttonCancel_->setEnabled(enabled);
}

Icd::ObjectPtr DetailEdit::target() const
{
    if (objectEdit_) {
        return objectEdit_->object();
    } else {
        return Icd::ObjectPtr();
    }
}

void DetailEdit::onContentChanged(const QString &key, const QVariant &value)
{
    Q_UNUSED(key);
    Q_UNUSED(value);
    if (!objectEdit_ || isHidden()) {
        return;
    }

    if (objectEdit_->blocking()) {
        return;
    }

    setButtonsEnabled(true);
}

void DetailEdit::restoreContent()
{
    if (!objectEdit_ || isHidden()) {
        return;
    }

    objectEdit_->restoreContent();

    setButtonsEnabled(false);
}

void DetailEdit::removeEdit()
{
    if (objectEdit_) {
        layoutEdit_->removeWidget(objectEdit_);
        objectEdit_->disconnect();
        objectEdit_->deleteLater();
        objectEdit_ = nullptr;
    }

    setButtonsEnabled(false);
}

void DetailEdit::changeEdit(int itemType)
{
    if (!object_ || object_->objectType() != Icd::ObjectItem) {
        Q_ASSERT(false);
        return;
    }

    removeEdit();

    const Icd::ItemPtr item = JHandlePtrCast<Icd::Item>(object_);
    Icd::ItemPtr newItem;
    if (itemType == item->type()) {
        newItem = JHandlePtrCast<Icd::Item>(item->copy());
        newItem->setParent(item->parent());
        newItem->setId(item->id());
    } else {
        newItem = Icd::Item::create(item->id(), Icd::ItemType(itemType), item->parent());
        if (!newItem) {
            Q_ASSERT(false);
            return;
        }
        *static_cast<Icd::Object*>(newItem.get()) = *static_cast<Icd::Object*>(item.get());
    }

    if (!newItem) {
        Q_ASSERT(false);
        return;
    }

    objectEdit_ = ObjectEdit::create(newItem);
    if (!objectEdit_) {
        Q_ASSERT(false);
        return;
    }

    connect(objectEdit_, &ObjectEdit::contentChanged, this, &DetailEdit::onContentChanged);
    if (object_->objectType() == Icd::ObjectItem) {
        connect(objectEdit_, &ObjectEdit::itemTypeChanged, this, &DetailEdit::changeEdit);
    }

    layoutEdit_->addWidget(objectEdit_);

    if (!objectEdit_->init()) {
        Q_ASSERT(false);
    }

    setButtonsEnabled(true);
}

}
