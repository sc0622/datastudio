#include "precomp.h"
#include "MonitorTreeView.h"

namespace Monitor {

TreeView::TreeView(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vertLyoutMain = new QVBoxLayout(this);
    vertLyoutMain->setContentsMargins(0, 0, 0, 0);
    vertLyoutMain->setSpacing(0);

    treeView_ = new Icd::CoreTreeWidget(this);
    treeView_->setTreeMode(Icd::CoreTreeWidget::TreeModeMonitor);
    treeView_->setBindTableType(Icd::CoreTreeWidget::BindOnlyRecv);
    vertLyoutMain->addWidget(treeView_);

    jnotify->on("monitor.toolbar.database.config", this, [=](JNEvent &){
        QVariantList args;
        args << "monitor" << qVariantFromValue((void*)this);
        jnotify->send("database.config", args);
    });
    connect(treeView_, &Icd::CoreTreeWidget::itemPressed, this, [=](QStandardItem *item){
        jnotify->send("monitor.tree.item.pressed", qVariantFromValue((void*)item));
    });
    connect(treeView_, &Icd::CoreTreeWidget::itemClicked, this, [=](QStandardItem *item){
        jnotify->send("monitor.tree.item.clicked", qVariantFromValue((void*)item));
    });
    connect(treeView_, &Icd::CoreTreeWidget::currentItemChanged, this,
            [=](QStandardItem *current, QStandardItem *previous){
        QVariantList args;
        args.append(qVariantFromValue((void*)current));
        args.append(qVariantFromValue((void*)previous));
        jnotify->send("monitor.tree.item.currentchanged", args);
    });
    connect(treeView_, &Icd::CoreTreeWidget::itemUnloaded, this,
            [=](QStandardItem *item, QStandardItem *tableItem){
        QVariantList args;
        args.append(qVariantFromValue((void*)item));
        args.append(qVariantFromValue((void*)tableItem));
        jnotify->send("monitor.tree.item.unloaded", args);
    });
    connect(treeView_, &Icd::CoreTreeWidget::channelBound, this,
            [=](QStandardItem *item, const QString &channelId){
        QVariantList args;
        args.append(qVariantFromValue((void*)item));
        args.append(channelId);
        jnotify->send("monitor.tree.channel.bound", args);
    });
    connect(treeView_, &Icd::CoreTreeWidget::channelUnbound, this,
            [=](QStandardItem *item, const QString &channelId){
        QVariantList args;
        args.append(qVariantFromValue((void*)item));
        args.append(channelId);
        jnotify->send("monitor.tree.channel.unbound", args);
    });
    connect(treeView_, &Icd::CoreTreeWidget::channelChanged, this,
            [=](QStandardItem *item, const QString &channelId){
        QVariantList args;
        args.append(qVariantFromValue((void*)item));
        args.append(channelId);
        jnotify->send("monitor.tree.channel.changed", args);
    });
    connect(treeView_, &Icd::CoreTreeWidget::unbindItem, this,
            [=](QStandardItem *item, QStandardItem *tableItem){
        QVariantList args;
        args.append(qVariantFromValue((void*)item));
        args.append(qVariantFromValue((void*)tableItem));
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
        args.append(qVariantFromValue((void*)this));
        jnotify->send("database.config", args);
    });
    jnotify->on("monitor.toolbar.tree.loadDeep", this, [=](JNEvent &event){
        const int deep = event.argument().toInt();
        JMain::instance()->setOption("monitor", "option.tree.loadDeep", deep);
    });
    jnotify->on("monitor.toolbar.tree.showOffset", this, [=](JNEvent &event){
        const bool checked = event.argument().toBool();
        treeView_->setShowAttribute(Icd::CoreTreeWidget::ShowOffset, checked);
        JMain::instance()->setOption("monitor", "option.tree.showOffset", checked);
    });
    jnotify->on("monitor.toolbar.tree.showType", this, [=](JNEvent &event){
        const bool checked = event.argument().toBool();
        treeView_->setShowAttribute(Icd::CoreTreeWidget::ShowType, checked);
        JMain::instance()->setOption("monitor", "option.tree.showType", checked);
    });
    jnotify->on("monitor.toolbar.tree.showOrignal", this, [=](JNEvent &event){
        const QVariantList args = event.argument().toList();
        if (args.count() < 1) {
            return;
        }
        treeView_->setShowAttribute(Icd::CoreTreeWidget::ShowData, args.at(0).toBool());
        if (args.count() < 2) {
            JMain::instance()->setOption("monitor", "option.tree.showOrignal", 0);
        } else {
            const int radix = args.at(1).toInt();
            treeView_->setDataFormat(radix);
            JMain::instance()->setOption("monitor", "option.tree.showOrignal", radix);
        }
    });
    jnotify->on("monitor.toolbar.tree.showData", this, [=](JNEvent &event){
        const bool checked = event.argument().toBool();
        treeView_->setShowAttribute(Icd::CoreTreeWidget::ShowData, checked);
        JMain::instance()->setOption("moitor", "option.tree.showData", checked);
    });
    jnotify->on("monitor.toolbar.tree.showReal", this, [=](JNEvent &event){
        const bool checked = event.argument().toBool();
        treeView_->setShowAttribute(Icd::CoreTreeWidget::ShowValue, checked);
        JMain::instance()->setOption("monitor", "option.tree.showValue", checked);
    });
    jnotify->on("monitor.toolbar.tree.showDesc", this, [=](JNEvent &event){
        const bool checked = event.argument().toBool();
        treeView_->setShowAttribute(Icd::CoreTreeWidget::ShowSpec, checked);
        JMain::instance()->setOption("monitor", "option.tree.showDesc", checked);
    });
    jnotify->on("monitor.toolbar.tree.flushToggle", this, [=](JNEvent &event){
        const bool checked = event.argument().toBool();
        treeView_->setRunning(checked);
        JMain::instance()->setOption("monitor", "option.tree.flushEnabled", checked);
    });
    jnotify->on("monitor.toolbar.tree.flushPeriod", this, [=](JNEvent &){
        QInputDialog inputDlg(this);
        inputDlg.setWindowTitle(tr("Input period of flushing"));
        inputDlg.setLabelText(tr("Flushing period:"));
        inputDlg.setOkButtonText(tr("Ok"));
        inputDlg.setCancelButtonText(tr("Cancel"));
        inputDlg.setInputMode(QInputDialog::TextInput);
        inputDlg.setIntRange(5, 1000000000);
        inputDlg.setIntValue(treeView_->intervalUpdate());
        inputDlg.resize(300, inputDlg.sizeHint().height());
        if (!inputDlg.exec() == QDialog::Accepted) {
            return;
        }
        treeView_->setIntervalUpdate(inputDlg.intValue());
        JMain::instance()->setOption("monitor", "option.tree.flushPeriod",
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
    treeView_->setShowAttribute((Icd::CoreTreeWidget::ShowAttribute)attr, on);
}

void TreeView::setOrigValueRadix(int radix)
{
    if (radix < 2 || radix > 36) {
        treeView_->setShowAttribute(Icd::CoreTreeWidget::ShowData, false);
    } else {
        treeView_->setShowAttribute(Icd::CoreTreeWidget::ShowData, true);
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
