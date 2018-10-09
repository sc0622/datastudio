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
    connect(treeView_, &Icd::JProtoTreeView::itemUpdated, this,
            [=](QStandardItem *item, bool unloaded, bool removed){
        QVariantList args;
        args.append(qVariantFromValue(static_cast<void*>(item)));
        args.append(unloaded);
        args.append(removed);
        args.append(qVariantFromValue(static_cast<void*>(treeView_->currentItem())));
        jnotify->send("edit.tree.item.updated", args);
    });
    connect(treeView_, &Icd::JProtoTreeView::itemUnloaded, this,
            [=](QStandardItem *item, QStandardItem *tableItem){
        QVariantList args;
        args.append(qVariantFromValue(static_cast<void*>(item)));
        args.append(qVariantFromValue(static_cast<void*>(tableItem)));
        jnotify->send("edit.tree.item.unloaded", args);
    });
    connect(treeView_, &Icd::JProtoTreeView::requestAdd, this,
            [=](QStandardItem *item, const QVariant &data){
        if (!item) {
            return;
        }
        //
        QVariantList args;
        args.append(qVariantFromValue(static_cast<void*>(item)));   // item
        args.append(data);  // data
        jnotify->send("edit.tree.request.add", args);
    });

    jnotify->on("edit.parser.changed", this, [=](JNEvent &){
        if (!updateParser()) {
            qDebug() << "edit: update parser failure!";
        }
    });
    jnotify->on("edit.parser.inst", this, [=](JNEvent &event){
        Icd::JParserPtrHandle *handle = jVariantFromVoid<Icd::JParserPtrHandle>(event.argument());
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
        save(false);
    });
    jnotify->on("edit.toolbar.tree.saveas", this, [=](JNEvent &){
        save(true);
    });
    jnotify->on("edit.toolbar.window.tree", this, [=](JNEvent &event){
        setVisible(event.argument().toBool());
    });
    jnotify->on("edit.tree.current.object", this, [=](JNEvent &event){
        const QVariantList args = event.argument().toList();
        if (args.size() != 2) {
            return;
        }
        Icd::RootPtr root = treeView_->protoRoot();
        if (!root) {
            return;
        }
        Icd::ObjectPtr *objectPtr = jVariantFromVoid<Icd::ObjectPtr>(args[0]);
        if (!objectPtr) {
            return;
        }
        QStandardItem *item = jVariantFromVoid<QStandardItem>(args[1]);
        if (!item) {
            return;
        }
        *objectPtr = treeView_->findObject(item, false);
        event.setReturnValue(true);
    });
    jnotify->on("edit.detail.changed", this, [=](JNEvent &event){
        const QVariantList args = event.argument().toList();
        if (args.size() != 4) {
            return;
        }
        const QString action = args[0].toString();
        const int currentRow = args[1].toInt();
        Icd::ObjectPtr *object = jVariantFromVoid<Icd::ObjectPtr>(args[2]);
        const QVariant data = args[3];
        //
        if (action == "insert") {
            treeView_->insertRow(currentRow, *object, data);
        } else if (action == "update") {
            treeView_->updateRow(currentRow, *object, data);
        } else if (action == "apply") {
            treeView_->applyInsert(*object);
        } else if (action == "cancel") {
            treeView_->cancelInsert();
        } else if (action == "remove") {
            treeView_->removeRow(currentRow, *object, data);
        }
    });
    jnotify->on("edit.toolbar.item.add", this, [=](JNEvent &){
        QStandardItem *currentItem = treeView_->currentItem();
        if (!currentItem) {
            return;
        }
        //
        QVariant data;
        if (currentItem->type() == Icd::TreeItemTypeTable) {
            data = Icd::NumericI8;
        }
        //
        QVariantList args;
        args.append(qVariantFromValue(static_cast<void*>(currentItem)));   // item
        args.append(data);  // data
        jnotify->send("edit.tree.request.add", args);
    });
}

TreeView::~TreeView()
{

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

    treeView_->setParser(parser);

    return treeView_->loadData();
}

void TreeView::save(bool saveAs)
{
    // check unloaded any protocol
    QStandardItem *unloadedItem = treeView_->unloadedItem();
    if (unloadedItem) {
        const Icd::ObjectPtr object = treeView_->findObject(unloadedItem);
        int result = QMessageBox::warning(this, tr("Warning"),
                                          tr("The protocol is not fully loaded, "
                                             "and saving will lose part of the protocol."
                                             "Continue saving?\n\n%1").arg(nameDomainOfObject(object)),
                                          QMessageBox::Yes | QMessageBox::No);
        if (result != QMessageBox::Yes) {
            return;
        }
    }

    Icd::ParserPtr parser;

    if (saveAs) {
        QString selected("JSON file (*.json)");
        const QString filePath = QFileDialog::getSaveFileName(
                    this, tr("Save as (Notice: will save you selected item and children)"),
                    QStandardPaths::locate(QStandardPaths::DesktopLocation, QString(), QStandardPaths::LocateDirectory)
                    + "untitled.json",
                    QString("JSON file (*.json);;XML file (*.xml)"), &selected);
        if (filePath.isEmpty()) {
            return;
        }
        Json::Value config;
        config["sourceType"] = "file";
        config["filePath"] = filePath.toStdString();
        parser = Icd::Parser::create(config);
    } else {
        parser = treeView_->parser();
    }

    if (!parser) {
        Q_ASSERT(false);
        return;
    }

    const Icd::RootPtr root = treeView_->protoRoot();
    if (!root) {
        return;
    }

    if (!parser->save(root)) {
        QMessageBox::warning(this, tr("Warning"), tr("Protocol is saved failed!"));
        return;
    }

    QMessageBox::information(this, tr("Notice"), tr("Protocol is saved successfully!"));
}

QString TreeView::nameDomainOfObject(const Icd::ObjectPtr &object) const
{
    if (!object) {
        return QString("<?>");
    }

    std::string name = object->name();
    QStringList sections(QString::fromStdString(name.empty() ? "?" : name));

    Icd::Object *parent = object->parent();
    while (parent && parent->objectType() > Icd::ObjectRoot) {
        name = parent->name();
        sections.append(QString::fromStdString(name.empty() ? "?" : name));
        parent = parent->parent();
    }

    return sections.join("@");
}

}
