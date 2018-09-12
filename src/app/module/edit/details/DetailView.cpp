#include "precomp.h"
#include "DetailView.h"
#include "DetailTable.h"
#include "DetailEdit.h"

namespace Edit {

DetailView::DetailView(QWidget *parent)
    : QWidget(parent)
{
    currentData_.item = nullptr;
    currentData_.object = Icd::ObjectPtr();

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

    connect(detailTable_, &DetailTable::currentItemChanged,
            this, &DetailView::onCurrentItemChanged);

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

void DetailView::updateView(QStandardItem *item)
{
    detailTable_->resetView();
    detailEdit_->resetView();

    currentData_.item = item;
    currentData_.object = nullptr;

    if (!item) {
        return;
    }

    Icd::JHandleScope<Icd::Object> handle;
    const QString domain = item->data(Icd::TreeItemDomainRole).toString();
    QVariantList args;
    args.append(qVariantFromValue(static_cast<void*>(&handle)));
    args.append(domain);
    if (!jnotify->send("edit.tree.current.object", args).toBool() || !handle.ptr) {
        return;
    }
    detailTable_->updateView(handle.ptr);
    detailEdit_->updateView(handle.ptr);
}

void DetailView::triggerEdit(QStandardItem *item, int editAction, const QVariant &data)
{
    if (!item) {
        return;
    }
}

void DetailView::onCurrentItemChanged(const QVariant &index)
{
    detailEdit_->updateView(detailTable_->object(), index);
}

}
