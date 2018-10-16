#include "precomp.h"
#include "DetailView.h"
#include "DetailTable.h"
#include "DetailEdit.h"

namespace Edit {

DetailView::DetailView(QWidget *parent)
    : QWidget(parent)
    , treeItem_(nullptr)
    , modified_(false)
{
    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);
    vertLayoutMain->setSpacing(0);

    splitterMain_ = new JSplitter(QList<double>() << 3 << 1, this);
    splitterMain_->setObjectName("Edit::detail::splitterMain");
    vertLayoutMain->addWidget(splitterMain_);

    detailTable_ = new DetailTable(this);
    splitterMain_->addWidget(detailTable_);

    detailEdit_ = new DetailEdit(this);
    splitterMain_->addWidget(detailEdit_);

    connect(detailTable_, &DetailTable::selectedChanged, this, &DetailView::onSelectedChanged);
    connect(detailTable_, &DetailTable::rowMoved, this, &DetailView::onRowMoved);
    connect(detailTable_, &DetailTable::requestInsert, this, &DetailView::onRequestInsert);
    connect(detailTable_, &DetailTable::requestPast, this, &DetailView::onRequestPast);
    connect(detailEdit_, &DetailEdit::applied, this, &DetailView::onApplied);
    connect(detailEdit_, &DetailEdit::canceled, this, &DetailView::onCanceled);

    detailEdit_->hide();
}

DetailView::~DetailView()
{
    JMain::saveWidgetState(splitterMain_);
}

bool DetailView::isModified() const
{
    return modified_;
}

bool DetailView::init()
{
    bool result = true;

    JMain::restoreWidgetState(splitterMain_);

    return result;
}

void DetailView::requestAdd(QStandardItem *item, const QVariant &data)
{
    if (!object_ || !item) {
        return;
    }

    insertRow(detailTable_->rowCount(), data);
}

void DetailView::moveCurrentRow(bool up)
{
    detailTable_->moveCurrentRow(up);
}

void DetailView::updateView(QStandardItem *item, bool unloaded)
{
    detailTable_->resetView();
    detailEdit_->resetView();

    setModified(false);
    treeItem_ = item;
    object_.reset();
    newObject_.reset();

    if (!item) {
        setAddEnabled(false);
        return;
    }

    Icd::ObjectPtr object;
    QVariantList args;
    args.append(qVariantFromValue(static_cast<void*>(&object)));
    args.append(qVariantFromValue(static_cast<void*>(item)));
    if (!jnotify->send("edit.tree.current.object", args).toBool() || !object) {
        return;
    }

    object_ = object;

    const bool isNew = item->data(Icd::TreeItemNewRole).toBool();
    if (isNew) {
        newObject_ = object_;
    }

    detailTable_->updateView(object_);
    detailEdit_->updateView(object_, false, isNew);

    bool addFlag = false;
    if (unloaded) {
        detailTable_->setContextMenuEnabled(false);
    } else {
        if (object_ && !object_->isSimpleItem()) {
            addFlag = true;
        }
    }

    setAddEnabled(!modified_ && addFlag);
}

void DetailView::removeRow(QStandardItem *item)
{
    if (!item || !object_) {
        return;
    }

    if (object_->isSimpleItem()) {
        return;
    }

    detailTable_->removeRow(item->row());

    setModified(false);
}

void DetailView::clearItem(QStandardItem *item)
{
    if (!item || !object_) {
        return;
    }

    if (object_->isSimpleItem()) {
        return;
    }

    detailTable_->cleanItem();

    setModified(false);
}

void DetailView::setModified(bool modified)
{
    if (modified != modified_) {
        modified_ = modified;
        emit modifiedChanged(modified);
    }

    setAddEnabled(!modified && detailTable_->contextMenuEnabled());
}

void DetailView::onSelectedChanged(const QVariant &index, const Icd::ObjectPtr &newObject)
{
    const bool isMultiRowSelected = detailTable_->isMultiRowSelected();
    if (isMultiRowSelected) {
        detailEdit_->resetView();
    } else {
        if (!index.isValid() || index.toInt() == -1) {
            cancelInsert(-1);
        } else if (newObject) {
            if (newObject == newObject_) {
                detailEdit_->updateView(newObject, false, true);
            } else {
                detailEdit_->updateView(detailTable_->object(), index);
            }
        } else if (newObject_) {
            cancelInsert(index.toInt());
        } else {
            //
            if (detailTable_->isMoved()) {
                detailTable_->cancel(index.toInt(), false);
            }
            //
            detailEdit_->updateView(detailTable_->object(), index);
        }
    }

    // update status of toolbar
    setAddEnabled(!modified_ && !isMultiRowSelected);
    updateMoveActionState();
}

void DetailView::onRowMoved(int previousRow, int currentRow, bool restore)
{
    Q_UNUSED(previousRow);
    Q_UNUSED(currentRow);
    Q_UNUSED(restore);
    detailEdit_->setButtonsEnabled(true);
    setModified(true);
    updateMoveActionState();
}

void DetailView::onRequestInsert(int row, const QVariant &data)
{
    insertRow(row, data);
}

void DetailView::onRequestPast(int row, const Icd::ObjectPtr &object, bool clone)
{
    insertRow(row, object, clone);
}

void DetailView::onApplied()
{
    if (!treeItem_ || !object_) {
        return;
    }

    const int selectedRow = detailTable_->selectedRow();
    const Icd::icd_uint64 selectedIndex = detailTable_->selectedIndex();
    const int originalRow = detailTable_->originalRow();
    QString action;
    if (newObject_) {
        if (treeItem_->data(Icd::TreeItemNewRole).toBool()) {
            action = "apply";
        } else {
            action = "insert";
        }
    } else {
        action = "update";
    }

    // save to core

    if (!saveObject()) {
        // restore tableview
        return;
    }

    Icd::ObjectPtr target = detailEdit_->target();

    // update tableview
    if (!detailTable_->apply(target, selectedRow)) {
        return;
    }

    // update treeview
    QVariantList args;
    // action
    args.append(action);
    // sourceRow
    args.append(originalRow == -1 ? selectedRow : originalRow);
    // targetRow
    args.append(originalRow == -1 ? -1 : selectedRow);
    // object handle
    args.append(qVariantFromValue(static_cast<void*>(&target)));
    // currentIndex
    args.append(selectedIndex);
    // send
    jnotify->send("edit.detail.apply", args);

    setModified(false);
}

void DetailView::onCanceled()
{
    cancelInsert(-1);
}

void DetailView::insertRow(int row, const QVariant &data)
{
    if (!object_) {
        return;
    }

    detailTable_->cancel(-1);
    detailTable_->clearSelection();
    newObject_.reset();

    switch (object_->rtti()) {
    case Icd::ObjectRoot:
    {
        auto newVehicle = std::make_shared<Icd::Vehicle>(object_.get());
        newObject_ = newVehicle;
        detailTable_->insertRow(row, newVehicle);
        break;
    }
    case Icd::ObjectVehicle:
    {
        auto newSystem = std::make_shared<Icd::System>(object_.get());
        newObject_ = newSystem;
        detailTable_->insertRow(row, newSystem);
        break;
    }
    case Icd::ObjectSystem:
    case Icd::ObjectFrame:
    {
        auto newTable = std::make_shared<Icd::Table>(object_.get());
        newObject_ = newTable;
        detailTable_->insertRow(row, newTable);
        break;
    }
    case Icd::ObjectComplex:
    {
        auto complex = JHandlePtrCast<Icd::ComplexItem>(object_);
        if (!complex) {
            return;
        }

        auto newItem = Icd::Item::create(Icd::ItemType(data.toInt()), complex->table().get());
        newObject_ = newItem;
        detailTable_->insertRow(row, newItem);
        break;
    }
    case Icd::ObjectTable:
    default:
    {
        auto newItem = Icd::Item::create(Icd::ItemType(data.toInt()), object_.get());
        newObject_ = newItem;
        detailTable_->insertRow(row, newItem);
        break;
    }}

    setModified(true);
}

void DetailView::insertRow(int row, const Icd::ObjectPtr &object, bool clone)
{
    if (!object_ || !object) {
        return;
    }

    detailTable_->cancel(-1);
    detailTable_->clearSelection();
    newObject_.reset();

    switch (object_->rtti()) {
    case Icd::ObjectRoot:
    {
        if (object->objectType() != Icd::ObjectVehicle) {
            return;
        }

        newObject_ = createNew(object, clone, object_);
        detailTable_->insertRow(row, JHandlePtrCast<Icd::Vehicle>(newObject_));
        break;
    }
    case Icd::ObjectVehicle:
    {
        if (object->objectType() != Icd::ObjectSystem) {
            return;
        }

        newObject_ = createNew(object, clone, object_);
        detailTable_->insertRow(row, JHandlePtrCast<Icd::System>(newObject_));
        break;
    }
    case Icd::ObjectSystem:
    case Icd::ObjectFrame:
    {
        if (object->objectType() != Icd::ObjectTable) {
            return;
        }

        newObject_ = createNew(object, clone, object_);
        detailTable_->insertRow(row, JHandlePtrCast<Icd::Table>(newObject_));
        break;
    }
    case Icd::ObjectComplex:
    {
        auto complex = JHandlePtrCast<Icd::ComplexItem>(object_);
        if (!complex) {
            return;
        }

        if (object->objectType() != Icd::ObjectItem) {
            return;
        }

        newObject_ = createNew(object, clone, object_);
        detailTable_->insertRow(row, JHandlePtrCast<Icd::Item>(newObject_));
        break;
    }
    case Icd::ObjectTable:
    default:
    {
        if (object->objectType() != Icd::ObjectItem) {
            return;
        }

        newObject_ = createNew(object, clone, object_);
        detailTable_->insertRow(row, JHandlePtrCast<Icd::Item>(newObject_));
        break;
    }}

    setModified(true);
}

bool DetailView::saveObject()
{
    if (!treeItem_) {
        return false;
    }

    if (newObject_) {
        int currentRow = -1;
        if (treeItem_->data(Icd::TreeItemNewRole).toBool()) {
            currentRow = treeItem_->row();
        } else {
            currentRow = detailTable_->selectedRow();
        }

        Icd::ObjectPtr target = detailEdit_->target();
        if (!target) {
            return false;
        }

        Icd::Object *parent = target->parent();
        if (!parent) {
            return false;
        }

        switch (target->objectType()) {
        case Icd::ObjectVehicle:
        {
            auto vehicle = JHandlePtrCast<Icd::Vehicle>(target);
            if (!vehicle) {
                return false;
            }
            auto root = dynamic_cast<Icd::Root*>(parent);
            if (!root) {
                return false;
            }
            root->insertVehicle(currentRow, vehicle);
            break;
        }
        case Icd::ObjectSystem:
        {
            auto system = JHandlePtrCast<Icd::System>(target);
            if (!system) {
                return false;
            }
            auto vehicle = dynamic_cast<Icd::Vehicle*>(parent);
            if (!vehicle) {
                return false;
            }
            vehicle->insertSystem(currentRow, system);
            break;
        }
        case Icd::ObjectTable:
        {
            auto table = JHandlePtrCast<Icd::Table>(target);
            if (!table) {
                return false;
            }
            if (parent->rtti() == Icd::ObjectSystem) {
                auto system = dynamic_cast<Icd::System*>(parent);
                if (!system) {
                    return false;
                }
                system->insertTable(currentRow, table);
            } else if (parent->rtti() == Icd::ObjectFrame) {
                auto frame = dynamic_cast<Icd::FrameItem*>(parent);
                if (!frame) {
                    return false;
                }
                frame->addTable(table);
            }
            break;
        }
        case Icd::ObjectItem:
        {
            auto item = JHandlePtrCast<Icd::Item>(target);
            if (!item) {
                return false;
            }
            auto table = dynamic_cast<Icd::Table*>(parent);
            if (!table) {
                return false;
            }
            table->insertItem(currentRow, item);
            break;
        }
        default:
            break;
        }

        newObject_.reset();
    } else {
        Icd::ObjectPtr target = detailEdit_->target();
        if (!target) {
            return false;
        }

        Icd::Object *parent = target->parent();
        if (!parent) {
            return false;
        }

        Icd::icd_uint64 currentIndex = 0;
        std::string tableId;
        int currentRow = detailTable_->selectedRow();
        if (currentRow == -1) {
            if (!treeItem_) {
                return false;
            }

            currentRow = treeItem_->row();
            if (currentRow < 0) {
                return false;
            }

            if (parent->rtti() == Icd::ObjectFrame) {
                if (target->rtti() != Icd::ObjectTable) {
                    return false;
                }
                auto table = JHandlePtrCast<Icd::Table>(target);
                if (!table) {
                    return false;
                }
                currentIndex = table->frameCode();
                tableId = table->id();
            } else {
                currentIndex = Icd::icd_uint64(currentRow);
            }
        } else {
            const int originalRow = detailTable_->originalRow();
            if (originalRow == -1) {
                currentIndex = detailTable_->selectedIndex();
                if (parent->rtti() == Icd::ObjectFrame) {
                    if (target->rtti() != Icd::ObjectTable) {
                        return false;
                    }
                    auto table = JHandlePtrCast<Icd::Table>(target);
                    if (!table) {
                        return false;
                    }
                    currentIndex = table->frameCode();
                    tableId = table->id();
                }
            } else {
                currentIndex = Icd::icd_uint64(originalRow);
            }
        }

        if (currentRow == -1 && tableId.empty()) {
            return false;
        }

        //
        if (detailTable_->isMoved()) {
            parent->moveChild(int(currentIndex), currentRow);
        } else {
            if (tableId.empty()) {
                parent->replaceChild(currentIndex, target);
            } else {
                parent->replaceChild(tableId, target);
            }
        }

        //
        if (detailTable_->selectedRow() == -1 || object_->isSimpleItem()) {
            object_ = target;
        }
    }

    return true;
}

void DetailView::setAddEnabled(bool enabled)
{
    setActionEnabled("add", enabled);
}

void DetailView::setUpEnabled(bool enabled)
{
    setActionEnabled("up", enabled);
}

void DetailView::setDownEnabled(bool enabled)
{
    setActionEnabled("down", enabled);
}

void DetailView::setActionEnabled(const QString &action, bool enabled)
{
    jnotify->send("edit.toolbar.item.action", QVariantList() << action << enabled);
}

void DetailView::updateMoveActionState()
{
    bool upFlag(false), downFlag(false);
    const int rowCount = detailTable_->rowCount();
    const int currentRow = detailTable_->selectedRow();

    if (rowCount > 1 && currentRow >= 0) {
        if (currentRow == 0) {
            downFlag = true;
        } else if (currentRow == rowCount - 1) {
            upFlag = true;
        } else {
            upFlag = true;
            downFlag = true;
        }
    }

    setUpEnabled(upFlag);
    setDownEnabled(downFlag);
}

void DetailView::cancelInsert(int row)
{
    if (!treeItem_ || !object_) {
        return;
    }

    //
    detailTable_->cancel(row);
    //
    if (detailTable_->selectedRow() == -1) {
        detailEdit_->updateView(object_, false, false);
    } else if (newObject_) {
        const Icd::icd_uint64 selectedIndex = detailTable_->selectedIndex();
        detailEdit_->updateView(object_, QVariant(selectedIndex));
    } else {
        detailEdit_->restoreContent();
    }

    newObject_.reset();

    // update treeview
    if (treeItem_->data(Icd::TreeItemNewRole).toBool()) {
        jnotify->send("edit.detail.cancel");
    }

    setModified(false);
}

Icd::ObjectPtr DetailView::createNew(const Icd::ObjectPtr &copied, bool clone, const Icd::ObjectPtr &parent)
{
    if (!copied) {
        return Icd::ObjectPtr();
    }

    Icd::ObjectPtr newObject = clone ? copied->clone() : copied->copy();
    newObject->setParent(parent.get());
    newObject->setName(std::string());
    newObject->setMark(std::string());

    return newObject;
}

}
