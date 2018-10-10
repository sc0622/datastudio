#include "precomp.h"
#include "EditSetView.h"
#include "details/DetailView.h"

namespace Edit {

SetView::SetView(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);
    vertLayoutMain->setSpacing(0);

    stackedWidget_ = new QStackedWidget(this);
    vertLayoutMain->addWidget(stackedWidget_);

    detailView_ = new DetailView(this);
    stackedWidget_->addWidget(detailView_);

    jnotify->on("edit.toolbar.window.settings", this, [=](JNEvent &event){
        setVisible(event.argument().toBool());
    });
    jnotify->on("edit.tree.item.currentchanged", this, [=](JNEvent &event){
        QVariantList args = event.argument().toList();
        if (args.count() != 2) {
            return;
        }
        onTreeCurrentChanged(jVariantFromVoid<QStandardItem>(args.at(0)),
                             jVariantFromVoid<QStandardItem>(args.at(1)));
    });
    jnotify->on("edit.tree.item.updated", this, [=](JNEvent &event){
        QVariantList args = event.argument().toList();
        if (args.count() != 4) {
            return;
        }
        onTreeItemUpdated(jVariantFromVoid<QStandardItem>(args.at(0)),
                          args.at(1).toBool(),
                          args.at(2).toBool(),
                          jVariantFromVoid<QStandardItem>(args.at(3)));
    });
    jnotify->on("edit.tree.request.add", this, [=](JNEvent &event){
        const QVariantList args = event.argument().toList();
        if (args.size() != 2) {
            return;
        }
        onRequestAdd(jVariantFromVoid<QStandardItem>(args[0]), args[1]);
    });
    jnotify->on("edit.toolbar.item.up", this, [=](JNEvent &){
        detailView_->moveCurrentRow(true);
    });
    jnotify->on("edit.toolbar.item.down", this, [=](JNEvent &){
        detailView_->moveCurrentRow(false);
    });
}

SetView::~SetView()
{

}

bool SetView::init()
{
    bool result = true;

    detailView_->init();

    return result;
}

void SetView::onTreeCurrentChanged(QStandardItem *current, QStandardItem *previous)
{
    Q_UNUSED(previous);
    detailView_->updateView(current);
}

void SetView::onTreeItemUpdated(QStandardItem *item, bool unloaded, bool removed, QStandardItem *current)
{
    Q_UNUSED(unloaded);
    if (current) {
        if (removed) {
            detailView_->removeRow(item);
        } else {
            detailView_->updateView(item);
        }
    }
}

void SetView::onRequestAdd(QStandardItem *item, const QVariant &data)
{
    detailView_->requestAdd(item, data);
}

}
