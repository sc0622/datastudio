#include "precomp.h"
#include "MonitorTreeView.h"

namespace Monitor {

TreeView::TreeView(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vertLyoutMain = new QVBoxLayout(this);
    vertLyoutMain->setContentsMargins(0, 0, 0, 0);
    vertLyoutMain->setSpacing(0);

    treeView_ = new Icd::JProtoTreeView(this);
    treeView_->setTreeMode(Icd::JProtoTreeView::TreeModeMonitor);
    treeView_->setBindTableType(Icd::JProtoTreeView::BindOnlyRecv);
    vertLyoutMain->addWidget(treeView_);

    jnotify->on("monitor.toolbar.database.config", this, [=](JNEvent &){
        QVariantList args;
        args << "monitor" << qVariantFromValue(static_cast<void*>(this));
        jnotify->send("database.config", args);
    });
    connect(treeView_, &Icd::JProtoTreeView::itemPressed, this, [=](QStandardItem *item){
        jnotify->send("monitor.tree.item.pressed", qVariantFromValue(static_cast<void*>(item)));
    });
    connect(treeView_, &Icd::JProtoTreeView::itemClicked, this, [=](QStandardItem *item){
        jnotify->send("monitor.tree.item.clicked", qVariantFromValue(static_cast<void*>(item)));
    });
    connect(treeView_, &Icd::JProtoTreeView::currentItemChanged, this,
            [=](QStandardItem *current, QStandardItem *previous){
        QVariantList args;
        args.append(qVariantFromValue(static_cast<void*>(current)));
        args.append(qVariantFromValue(static_cast<void*>(previous)));
        jnotify->send("monitor.tree.item.currentchanged", args);
    });
    connect(treeView_, &Icd::JProtoTreeView::itemUnloaded, this,
            [=](QStandardItem *item, QStandardItem *tableItem){
        QVariantList args;
        args.append(qVariantFromValue(static_cast<void*>(item)));
        args.append(qVariantFromValue(static_cast<void*>(tableItem)));
        jnotify->send("monitor.tree.item.unloaded", args);
    });
    connect(treeView_, &Icd::JProtoTreeView::channelBound, this,
            [=](QStandardItem *item, const QString &channelId){
        QVariantList args;
        args.append(qVariantFromValue(static_cast<void*>(item)));
        args.append(channelId);
        jnotify->send("monitor.tree.channel.bound", args);
    });
    connect(treeView_, &Icd::JProtoTreeView::channelUnbound, this,
            [=](QStandardItem *item, const QString &channelId){
        QVariantList args;
        args.append(qVariantFromValue(static_cast<void*>(item)));
        args.append(channelId);
        jnotify->send("monitor.tree.channel.unbound", args);
    });
    connect(treeView_, &Icd::JProtoTreeView::channelChanged, this,
            [=](QStandardItem *item, const QString &channelId){
        QVariantList args;
        args.append(qVariantFromValue(static_cast<void*>(item)));
        args.append(channelId);
        jnotify->send("monitor.tree.channel.changed", args);
    });
    connect(treeView_, &Icd::JProtoTreeView::unbindItem, this,
            [=](QStandardItem *item, QStandardItem *tableItem){
        QVariantList args;
        args.append(qVariantFromValue(static_cast<void*>(item)));
        args.append(qVariantFromValue(static_cast<void*>(tableItem)));
        jnotify->send("monitor.tree.item.unbind", args);
    });

    jnotify->on("monitor.parser.changed", this, [=](JNEvent &){
        if (!updateParser()) {
            qDebug() << "monitor: update parser failure!";
        }
    });
    jnotify->on("monitor.parser.inst", this, [=](JNEvent &event){
        Icd::JParserPtrHandle *handle =
                jVariantFromVoid<Icd::JParserPtrHandle>(event.argument());
        if (!handle) {
            return;
        }
        handle->parser = treeView_->parser();
        event.setReturnValue(true);
    });
    jnotify->on("monitor.toolbar.database.config", this, [=](JNEvent &){
        QVariantList args;
        // module
        args.append("monitor");
        // receiver
        args.append(qVariantFromValue(static_cast<void*>(this)));
        jnotify->send("database.config", args);
    });
    jnotify->on("monitor.toolbar.tree.loadDeep", this, [=](JNEvent &event){
        treeView_->setLoadingDeep(event.argument().toInt());
    });
    jnotify->on("monitor.toolbar.tree.showOffset", this, [=](JNEvent &event){
        const bool checked = event.argument().toBool();
        treeView_->setShowAttribute(Icd::JProtoTreeView::ShowOffset, checked);
    });
    jnotify->on("monitor.toolbar.tree.showType", this, [=](JNEvent &event){
        const bool checked = event.argument().toBool();
        treeView_->setShowAttribute(Icd::JProtoTreeView::ShowType, checked);
    });
    jnotify->on("monitor.toolbar.tree.showOrignal", this, [=](JNEvent &event){
        const QVariantList args = event.argument().toList();
        if (args.count() < 1) {
            return;
        }
        treeView_->setShowAttribute(Icd::JProtoTreeView::ShowData, args.at(0).toBool());
        if (args.count() >= 2) {
            treeView_->setDataFormat(args.at(1).toInt());
        }
    });
    jnotify->on("monitor.toolbar.tree.showData", this, [=](JNEvent &event){
        const bool checked = event.argument().toBool();
        treeView_->setShowAttribute(Icd::JProtoTreeView::ShowData, checked);
        JMain::instance()->setOption("moitor", "option.tree.showData", checked);
    });
    jnotify->on("monitor.toolbar.tree.showReal", this, [=](JNEvent &event){
        const bool checked = event.argument().toBool();
        treeView_->setShowAttribute(Icd::JProtoTreeView::ShowValue, checked);
    });
    jnotify->on("monitor.toolbar.tree.showDesc", this, [=](JNEvent &event){
        const bool checked = event.argument().toBool();
        treeView_->setShowAttribute(Icd::JProtoTreeView::ShowSpec, checked);
    });
    jnotify->on("monitor.toolbar.flushToggle", this, [=](JNEvent &event){
        const bool checked = event.argument().toBool();
        treeView_->setRunning(checked);
    });
    jnotify->on("monitor.toolbar.flushPeriod", this, [=](JNEvent &){
        QInputDialog inputDlg(this);
        inputDlg.setWindowTitle(tr("Input period of flushing"));
        inputDlg.setLabelText(tr("Flushing period:"));
        inputDlg.setOkButtonText(tr("Ok"));
        inputDlg.setCancelButtonText(tr("Cancel"));
        inputDlg.setInputMode(QInputDialog::TextInput);
        inputDlg.setIntRange(5, 1000000000);
        inputDlg.setIntValue(treeView_->intervalUpdate());
        inputDlg.resize(300, inputDlg.sizeHint().height());
        if (inputDlg.exec() != QDialog::Accepted) {
            return;
        }
        treeView_->setIntervalUpdate(inputDlg.intValue());
        JMain::instance()->setOption("monitor", "option.flushPeriod",
                                     inputDlg.intValue());
    });
    jnotify->on("monitor.toolbar.tree.channel.binding", this, [=](JNEvent &){
        const QString filePath = QFileDialog::getOpenFileName(
                    this, tr("Select channel binding file"), QString(),
                    "XML file (*.xml)");
        if (filePath.isEmpty()) {
            return;
        }
        treeView_->bindingChannels(filePath);
    });
    jnotify->on("monitor.toolbar.tree.channel.unbinding", this, [=](JNEvent &){
        treeView_->unbindingChannels();
    });
    jnotify->on("monitor.toolbar.tree.channel.export", this, [=](JNEvent &){
        const QString filePath = QFileDialog::getSaveFileName(
                    this, tr("Save channel binding relations"), QString(), "XML file (*.xml)");
        if (filePath.isEmpty()) {
            return;
        }
        treeView_->exportBindingStatus(filePath);
    });
    jnotify->on("monitor.toolbar.tree.channel.runAll", this, [=](JNEvent &){
        treeView_->runAllChannels();
    });
    jnotify->on("monitor.toolbar.tree.channel.stopAll", this, [=](JNEvent &){
        treeView_->stopAllChannels();
    });
    jnotify->on("monitor.toolbar.window.tree", this, [=](JNEvent &event){
        setVisible(event.argument().toBool());
    });
}

TreeView::~TreeView()
{

}

bool TreeView::init()
{
    bool result = true;

    Json::Value option = JMain::instance()->option("monitor", "option.tree");
    if (option.isNull()) {
        return false;
    }
    // deep
    if (option.isMember("loadDeep")) {
        treeView_->setLoadingDeep(option["loadDeep"].asInt());
    }
    // flush-period
    if (option.isMember("flushPeriod")) {
        treeView_->setIntervalUpdate(option["flushPeriod"].asInt());
    }

    if (!updateParser()) {
        qDebug() << "monitor: update parser failure!";
        result = false;
    }

    return result;
}

void TreeView::setShowAttribute(int attr, bool on)
{
    treeView_->setShowAttribute(static_cast<Icd::JProtoTreeView::ShowAttribute>(attr), on);
}

void TreeView::setOrigValueRadix(int radix)
{
    if (radix < 2 || radix > 36) {
        treeView_->setShowAttribute(Icd::JProtoTreeView::ShowData, false);
    } else {
        treeView_->setShowAttribute(Icd::JProtoTreeView::ShowData, true);
        treeView_->setDataFormat(radix);
    }
}

bool TreeView::updateParser()
{
    treeView_->clearContents();

    const Json::Value config = JMain::instance()->option("monitor", "parser");
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

}
