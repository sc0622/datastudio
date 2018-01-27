#include "precomp.h"
#include "EditTreeView.h"

namespace Edit {

TreeView::TreeView(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vertLyoutMain = new QVBoxLayout(this);
    vertLyoutMain->setContentsMargins(0, 0, 0, 0);
    vertLyoutMain->setSpacing(0);

    d_treeView = new Icd::CoreTreeWidget(this);
    d_treeView->setTreeMode(Icd::CoreTreeWidget::TreeModeEdit);
    vertLyoutMain->addWidget(d_treeView);

    connect(d_treeView, &Icd::CoreTreeWidget::itemPressed, this, [=](QStandardItem *item){
        jnotify->send("edit.tree.item.pressed", qVariantFromValue((void*)item));
    });
    connect(d_treeView, &Icd::CoreTreeWidget::itemClicked, this, [=](QStandardItem *item){
        jnotify->send("edit.tree.item.clicked", qVariantFromValue((void*)item));
    });
    connect(d_treeView, &Icd::CoreTreeWidget::currentItemChanged, this,
            [=](QStandardItem *current, QStandardItem *previous){
        QVariantList args;
        args.append(qVariantFromValue((void*)current));
        args.append(qVariantFromValue((void*)previous));
        jnotify->send("edit.tree.item.currentchanged", args);
    });
    connect(d_treeView, &Icd::CoreTreeWidget::itemUnloaded, this,
            [=](QStandardItem *item, QStandardItem *tableItem){
        QVariantList args;
        args.append(qVariantFromValue((void*)item));
        args.append(qVariantFromValue((void*)tableItem));
        jnotify->send("edit.tree.item.unloaded", args);
    });

    jnotify->on("edit.parser.changed", this, [=](JNEvent &){
        if (!updateParser()) {
            qDebug() << "edit: update parser failure!";
        }
    });
    jnotify->on("edit.parser.inst", this, [=](JNEvent &event){
        Icd::JParserPtrHandle handle;
        handle.parser = d_treeView->parser();
        event.setReturnValue(qVariantFromValue((void *)&handle));
    });
}

TreeView::~TreeView()
{

}

bool TreeView::init()
{
    bool result = true;

    Json::Value config = JMain::instance()->option("edit/option", "deep");
    if (config.isNull()) {
        return false;
    }

    d_treeView->setLoadingDeep(config.asInt());

    if (!updateParser()) {
        qDebug() << "edit: update parser failure!";
        return false;
    }

    return result;
}

bool TreeView::updateParser()
{
    d_treeView->clearContents();

    const Json::Value config = JMain::instance()->option("edit", "parser");
    if (config.isNull()) {
        return false;
    }

    const Icd::ParserPtr parser = Icd::Parser::create(config);
    if (!parser) {
        return false;
    }

    d_treeView->setParser(parser);

    return d_treeView->loadData();
}

}
