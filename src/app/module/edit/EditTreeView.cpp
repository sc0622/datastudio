#include "precomp.h"
#include "EditTreeView.h"
#include "EditCreateDlg.h"

namespace Edit {

TreeView::TreeView(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vertLyoutMain = new QVBoxLayout(this);
    vertLyoutMain->setContentsMargins(0, 0, 0, 0);
    vertLyoutMain->setSpacing(0);

    treeView_ = new Icd::JProtoTreeView(this);
    treeView_->setTreeMode(Icd::JProtoTreeView::TreeModeEdit);
    vertLyoutMain->addWidget(treeView_);

    jnotify->on("edit.toolbar.database.config", this, [=](JNEvent &){
        QVariantList args;
        args << "edit" << qVariantFromValue(static_cast<void*>(this));
        jnotify->send("database.config", args);
    });
    jnotify->on("edit.toolbar.database.create", this, [=](JNEvent &){
        EditCreateDlg dialog(this);
        if (dialog.exec() != QDialog::Accepted) {
            return;
        }
        // notify edit module
        jnotify->post("edit.parser.changed", this);
    });
    connect(treeView_, &Icd::JProtoTreeView::itemPressed, this, [=](QStandardItem *item){
        jnotify->send("edit.tree.item.pressed", qVariantFromValue(static_cast<void*>(item)));
    });
    connect(treeView_, &Icd::JProtoTreeView::itemClicked, this, [=](QStandardItem *item){
        jnotify->send("edit.tree.item.clicked", qVariantFromValue(static_cast<void*>(item)));
    });
    connect(treeView_, &Icd::JProtoTreeView::currentItemChanged, this,
            [=](QStandardItem *current, QStandardItem *previous){
        QVariantList args;
        args.append(qVariantFromValue(static_cast<void*>(current)));
        args.append(qVariantFromValue(static_cast<void*>(previous)));
        jnotify->send("edit.tree.item.currentchanged", args);
    });
    connect(treeView_, &Icd::JProtoTreeView::itemUnloaded, this,
            [=](QStandardItem *item, QStandardItem *tableItem){
        QVariantList args;
        args.append(qVariantFromValue(static_cast<void*>(item)));
        args.append(qVariantFromValue(static_cast<void*>(tableItem)));
        jnotify->send("edit.tree.item.unloaded", args);
    });
    connect(treeView_, &Icd::JProtoTreeView::editItemTriggered, this,
            [=](QStandardItem *item, Icd::JProtoTreeView::EditAction action, const QVariant &data){
        if (!item) {
            return;
        }
        //
        QVariantList args;
        args.append(qVariantFromValue(static_cast<void*>(item)));   // item
        args.append(int(action)); // EditAction
        args.append(data);  // data
        jnotify->send("edit.tree.edit.triggered", args);
    });

    jnotify->on("edit.parser.changed", this, [=](JNEvent &){
        if (!updateParser()) {
            qDebug() << "edit: update parser failure!";
        }
    });
    jnotify->on("edit.parser.inst", this, [=](JNEvent &event){
        Icd::JParserPtrHandle *handle =
                jVariantFromVoid<Icd::JParserPtrHandle>(event.argument());
        if (!handle) {
            return;
        }
        handle->parser = treeView_->parser();
        event.setReturnValue(true);
    });
    jnotify->on("edit.toolbar.tree.loadDeep", this, [=](JNEvent &event){
        treeView_->setLoadingDeep(event.argument().toInt());
    });
    jnotify->on("edit.toolbar.tree.showOffset", this, [=](JNEvent &event){
        const bool checked = event.argument().toBool();
        treeView_->setShowAttribute(Icd::JProtoTreeView::ShowOffset, checked);
    });
    jnotify->on("edit.toolbar.tree.showType", this, [=](JNEvent &event){
        const bool checked = event.argument().toBool();
        treeView_->setShowAttribute(Icd::JProtoTreeView::ShowType, checked);
    });
    jnotify->on("edit.toolbar.tree.copy", this, [=](JNEvent &){
        //
    });
    jnotify->on("edit.toolbar.tree.save", this, [=](JNEvent &){
        // check unloaded any protocol
        if (treeView_->hasUnloadedItem()) {
            int result = QMessageBox::warning(this, tr("Warning"),
                                              tr("The protocol is not fully loaded, "
                                                 "and saving will lose part of the protocol.\n"
                                                 "Continue saving?"),
                                              QMessageBox::Yes | QMessageBox::No);
            if (result != QMessageBox::Yes) {
                return;
            }
        }
        //
        const Icd::RootPtr root = treeView_->protoRoot();
        if (!root) {
            return;
        }
        //
        Icd::ParserPtr parser = treeView_->parser();
        if (!parser) {
            Q_ASSERT(false);
            return;
        }
        //
        if (!parser->save(root)) {
            QMessageBox::warning(this, tr("Warning"), tr("Protocol is saved failed!"));
            return;
        }
        QMessageBox::information(this, tr("Notice"), tr("Protocol is saved successfully!"));
    });
    jnotify->on("edit.toolbar.tree.saveas", this, [=](JNEvent &){
        //
    });
    jnotify->on("edit.toolbar.window.tree", this, [=](JNEvent &event){
        setVisible(event.argument().toBool());
    });
    jnotify->on("edit.tree.current.object", this, [=](JNEvent &event){
        const QVariantList args = event.argument().toList();
        Q_ASSERT(args.size() == 2);
        if (args.size() != 2) {
            return;
        }
        Icd::RootPtr root = treeView_->protoRoot();
        if (!root) {
            return;
        }
        Icd::JHandleScope<Icd::Object> *handle =
                jVariantFromVoid<Icd::JHandleScope<Icd::Object> >(args[0]);
        if (!handle) {
            return;
        }
        const QString domain = args[1].toString();
        if (domain.isEmpty()) {
            handle->ptr = root;
        } else {
            const Icd::ObjectPtr object = root->findByDomain(domain.toStdString());
            if (!object) {
                return;
            }
            handle->ptr = object;
        }
        event.setReturnValue(true);
    });
}

TreeView::~TreeView()
{
    Icd::ParserPtr parser = treeView_->parser();
    if (parser) {
        parser->endModify();
    }
}

bool TreeView::init()
{
    bool result = true;

    Json::Value option = JMain::instance()->option("edit", "option.tree");
    if (option.isNull()) {
        return false;
    }
    // deep
    if (option.isMember("loadDeep")) {
        treeView_->setLoadingDeep(option["loadDeep"].asInt());
    }

    if (!updateParser()) {
        qDebug() << "edit: update parser failure!";
        return false;
    }

    return result;
}

void TreeView::setShowAttribute(int attr, bool on)
{
    treeView_->setShowAttribute(static_cast<Icd::JProtoTreeView::ShowAttribute>(attr), on);
}

bool TreeView::updateParser()
{
    treeView_->clearContents();

    const Json::Value config = JMain::instance()->option("edit", "parser");
    if (config.isNull()) {
        return false;
    }

    const Icd::ParserPtr parser = Icd::Parser::create(config);
    if (!parser) {
        return false;
    }

    //
    parser->beginModify();

    treeView_->setParser(parser);

    return treeView_->loadData();
}

}
