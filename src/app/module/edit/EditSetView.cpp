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
    jnotify->on("edit.tree.edit.triggered", this, [=](JNEvent &event){
        const QVariantList args = event.argument().toList();
        if (args.size() != 3) {
            return;
        }
        QStandardItem *item = jVariantFromVoid<QStandardItem>(args[0]);
        const int editAction = args[1].toInt();
        const QVariant &data = args[2].data();
        onEditTriggered(item, editAction, data);
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

void SetView::onEditTriggered(QStandardItem *item, int editAction, const QVariant &data)
{
    detailView_->triggerEdit(item, editAction, data);
}

}
