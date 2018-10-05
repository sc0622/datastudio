#include "precomp.h"
#include "DetailView.h"
#include "DetailTable.h"
#include "DetailEdit.h"

namespace Edit {

DetailView::DetailView(QWidget *parent)
    : QWidget(parent)
    , treeItem_(nullptr)
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

    connect(detailTable_, &DetailTable::currentItemChanged, this, &DetailView::onCurrentItemChanged);
    connect(detailEdit_, &DetailEdit::applied, this, &DetailView::onApplied);
    connect(detailEdit_, &DetailEdit::canceled, this, &DetailView::onCanceled);

    detailEdit_->hide();
}

DetailView::~DetailView()
{
    JMain::saveWidgetState(splitterMain_);
}

bool DetailView::init()
{
    bool result = true;

    JMain::restoreWidgetState(splitterMain_);

    return result;
}

void DetailView::requestAdd(QStandardItem *item, const QVariant &data)
{
    Q_UNUSED(data);
    if (!object_ || !item) {
        return;
    }

    detailTable_->cancelInsert();
    newObject_.reset();

    insertRow(detailTable_->rowCount(), item, data);
}

void DetailView::updateView(QStandardItem *item)
{
    detailTable_->resetView();
    detailEdit_->resetView();

    treeItem_ = item;
    object_.reset();
    newObject_.reset();

    if (!item) {
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
}

void DetailView::removeRow(QStandardItem *item)
{
    if (!item) {
        return;
    }
}

void DetailView::cleanItem(QStandardItem *item)
{
    if (!item) {
        return;
    }
}

void DetailView::onCurrentItemChanged(const QVariant &index, const Icd::ObjectPtr &newObject)
{
    const bool isMultiRowSelected = detailTable_->isMultiRowSelected();
    if (isMultiRowSelected) {
        detailEdit_->resetView();
    } else {
        if (newObject && newObject == newObject_) {
            detailEdit_->updateView(newObject, false, true);
        } else {
            detailEdit_->updateView(detailTable_->object(), index);
        }
    }

    // update status of toolbar

    bool addFlag = true, upFlag = false, downFlag = false;

    if (isMultiRowSelected) {
        addFlag = false;
    } else {
        const int rowCount = detailTable_->rowCount();
        const int currentRow = detailTable_->currentRow();
        if (currentRow >= 0) {
            if (currentRow == 0) {
                downFlag = true;
            } else if (currentRow == rowCount - 1) {
                upFlag = true;
            } else {
                upFlag = true;
                downFlag = true;
            }
        }
    }

    QVariantList args;
    // add
    args.append(addFlag);
    // up
    args.append(upFlag);
    // down
    args.append(downFlag);
    // send
    jnotify->send("edit.toolbar.item.action", args);
}

void DetailView::onApplied()
{
    if (!treeItem_ || !object_) {
        return;
    }

    const int currentRow = detailTable_->currentRow();
    const Icd::icd_uint64 currentIndex = detailTable_->currentIndex();
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
    detailTable_->applyInsert();

    // update treeview
    QVariantList args;
    // action
    args.append(action);
    // row
    args.append(currentRow);
    // object handle
    args.append(qVariantFromValue(static_cast<void*>(&target)));
    // currentIndex
    args.append(currentIndex);
    // send
    jnotify->send("edit.detail.changed", args);
}

void DetailView::onCanceled()
{
    if (!treeItem_ || !object_) {
        return;
    }

    detailTable_->cancelInsert();
    detailEdit_->resetView();
    newObject_.reset();

    // notify treeview
    if (treeItem_->data(Icd::TreeItemNewRole).toBool()) {
        // update treeview
        QVariantList args;
        // action
        args.append("cancel");
        // row
        args.append(-1);
        // object handle
        args.append(QVariant::Invalid);
        // currentIndex
        args.append(QVariant::Invalid);
        // send
        jnotify->send("edit.detail.changed", args);
    }
}

void DetailView::insertRow(int row, QStandardItem *item, const QVariant &data)
{
    Q_UNUSED(data);

    if (!item) {
        return;
    }

    const int itemType = item->type();
    switch (itemType) {
    case Icd::TreeItemTypeRoot:
    {
        auto newVehicle = std::make_shared<Icd::Vehicle>(object_.get());
        newObject_ = newVehicle;
        detailTable_->insertRow(row, newVehicle);
        break;
    }
    case Icd::TreeItemTypeVehicle:
    {
        auto newSystem = std::make_shared<Icd::System>(object_.get());
        newObject_ = newSystem;
        detailTable_->insertRow(row, newSystem);
        break;
    }
    case Icd::TreeItemTypeSystem:
    {
        auto newTable = std::make_shared<Icd::Table>(object_.get());
        newObject_ = newTable;
        detailTable_->insertRow(row, newTable);
        break;
    }
    case Icd::TreeItemTypeTable:
    {
        auto newItem = Icd::Item::create(Icd::ItemType(data.toInt()));
        newItem->setParent(object_.get());
        newObject_ = newItem;
        detailTable_->insertRow(row, newItem);
        break;
    }
    default:
        break;
    }
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
            currentRow = detailTable_->currentRow();
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

        // replace
        Icd::icd_uint64 currentIndex = 0;
        int currentRow = detailTable_->currentRow();
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
            } else {
                currentIndex = Icd::icd_uint64(currentRow);
            }
        } else {
            currentIndex = detailTable_->currentIndex();
        }

        parent->replaceChild(currentIndex, target);

        object_ = target;
    }

    return true;
}

}
