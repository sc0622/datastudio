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

    d_stackedWidget = new QStackedWidget(this);
    vertLayoutMain->addWidget(d_stackedWidget);

    d_detailView = new DetailView(this);
    d_stackedWidget->addWidget(d_detailView);

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
}

SetView::~SetView()
{

}

bool SetView::init()
{
    bool result = true;

    d_detailView->init();

    return result;
}

void SetView::onTreeCurrentChanged(QStandardItem *current, QStandardItem *previous)
{
    Q_UNUSED(previous);
    d_detailView->updateView(current);
}

}
