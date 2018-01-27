#include "precomp.h"
#include "MonitorTreeView.h"

namespace Monitor {

TreeView::TreeView(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vertLyoutMain = new QVBoxLayout(this);
    vertLyoutMain->setContentsMargins(0, 0, 0, 0);
    vertLyoutMain->setSpacing(0);

    d_treeView = new Icd::CoreTreeWidget(this);
    d_treeView->setTreeMode(Icd::CoreTreeWidget::TreeModeMonitor);
    d_treeView->setBindTableType(Icd::CoreTreeWidget::BindOnlyRecv);
    vertLyoutMain->addWidget(d_treeView);

    connect(d_treeView, &Icd::CoreTreeWidget::itemPressed, this, [=](QStandardItem *item){
        jnotify->send("monitor.tree.item.pressed", qVariantFromValue((void*)item));
    });
    connect(d_treeView, &Icd::CoreTreeWidget::itemClicked, this, [=](QStandardItem *item){
        jnotify->send("monitor.tree.item.clicked", qVariantFromValue((void*)item));
    });
    connect(d_treeView, &Icd::CoreTreeWidget::currentItemChanged, this,
            [=](QStandardItem *current, QStandardItem *previous){
        QVariantList args;
        args.append(qVariantFromValue((void*)current));
        args.append(qVariantFromValue((void*)previous));
        jnotify->send("monitor.tree.item.currentchanged", args);
    });
    connect(d_treeView, &Icd::CoreTreeWidget::itemUnloaded, this,
            [=](QStandardItem *item, QStandardItem *tableItem){
        QVariantList args;
        args.append(qVariantFromValue((void*)item));
        args.append(qVariantFromValue((void*)tableItem));
        jnotify->send("monitor.tree.item.unloaded", args);
    });
    connect(d_treeView, &Icd::CoreTreeWidget::channelBound, this,
            [=](QStandardItem *item, const QString &channelId){
        QVariantList args;
        args.append(qVariantFromValue((void*)item));
        args.append(channelId);
        jnotify->send("monitor.tree.channel.bound", args);
    });
    connect(d_treeView, &Icd::CoreTreeWidget::channelUnbound, this,
            [=](QStandardItem *item, const QString &channelId){
        QVariantList args;
        args.append(qVariantFromValue((void*)item));
        args.append(channelId);
        jnotify->send("monitor.tree.channel.unbound", args);
    });
    connect(d_treeView, &Icd::CoreTreeWidget::channelChanged, this,
            [=](QStandardItem *item, const QString &channelId){
        QVariantList args;
        args.append(qVariantFromValue((void*)item));
        args.append(channelId);
        jnotify->send("monitor.tree.channel.changed", args);
    });
    connect(d_treeView, &Icd::CoreTreeWidget::unbindItem, this,
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
        Icd::JParserPtrHandle handle;
        handle.parser = d_treeView->parser();
        event.setReturnValue(qVariantFromValue((void *)&handle));
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
        JMain::instance()->setOption("monitor", "option/tree/deep", deep, true);
    });
    jnotify->on("monitor.toolbar.tree.showOffset", this, [=](JNEvent &event){
        const bool checked = event.argument().toBool();
        d_treeView->setShowAttribute(Icd::CoreTreeWidget::ShowOffset, checked);
        JMain::instance()->setOption("monitor", "option/tree/showOffset", checked, true);
    });
    jnotify->on("monitor.toolbar.tree.showType", this, [=](JNEvent &event){
        const bool checked = event.argument().toBool();
        d_treeView->setShowAttribute(Icd::CoreTreeWidget::ShowType, checked);
        JMain::instance()->setOption("monitor", "option/tree/showType", checked, true);
    });
    jnotify->on("monitor.toolbar.tree.showOrignal", this, [=](JNEvent &event){
        const QVariantList args = event.argument().toList();
        if (args.count() < 1) {
            return;
        }
        d_treeView->setShowAttribute(Icd::CoreTreeWidget::ShowData, args.at(0).toBool());
        if (args.count() < 2) {
            JMain::instance()->setOption("monitor", "option/tree/showOrignal", 0, true);
        } else {
            const int radix = args.at(1).toInt();
            d_treeView->setDataFormat(radix);
            JMain::instance()->setOption("monitor", "option/tree/showOrignal", radix, true);
        }
    });
    jnotify->on("monitor.toolbar.tree.showReal", this, [=](JNEvent &event){
        const bool checked = event.argument().toBool();
        d_treeView->setShowAttribute(Icd::CoreTreeWidget::ShowValue, checked);
        JMain::instance()->setOption("monitor", "option/tree/showValue", checked, true);
    });
    jnotify->on("monitor.toolbar.tree.showDesc", this, [=](JNEvent &event){
        const bool checked = event.argument().toBool();
        d_treeView->setShowAttribute(Icd::CoreTreeWidget::ShowSpec, checked);
        JMain::instance()->setOption("monitor", "option/tree/showDesc", checked, true);
    });
    jnotify->on("monitor.toolbar.tree.flushToggle", this, [=](JNEvent &event){
        const bool checked = event.argument().toBool();
        d_treeView->setRunning(checked);
        JMain::instance()->setOption("monitor", "option/tree/flushEnabled", checked, true);
    });
    jnotify->on("monitor.toolbar.tree.flushPeriod", this, [=](JNEvent &){
        QInputDialog inputDlg(this);
        inputDlg.setWindowTitle(tr("Input period of flushing"));
        inputDlg.setLabelText(tr("Flushing period:"));
        inputDlg.setOkButtonText(tr("Ok"));
        inputDlg.setCancelButtonText(tr("Cancel"));
        inputDlg.setInputMode(QInputDialog::TextInput);
        inputDlg.setIntRange(5, 1000000000);
        inputDlg.setIntValue(d_treeView->intervalUpdate());
        inputDlg.resize(300, inputDlg.sizeHint().height());
        if (!inputDlg.exec() == QDialog::Accepted) {
            return;
        }
        d_treeView->setIntervalUpdate(inputDlg.intValue());
        JMain::instance()->setOption("monitor", "option/tree/flushPeriod",
                                     inputDlg.intValue(), true);
    });
    jnotify->on("monitor.toolbar.tree.channel.binding", this, [=](JNEvent &){
        const QString filePath = QFileDialog::getOpenFileName(
                    this, tr("Select channel binding file"), QString(),
                    "XML file (*.xml)");
        if (filePath.isEmpty()) {
            return;
        }
        d_treeView->bindingChannels(filePath);
    });
    jnotify->on("monitor.toolbar.tree.channel.unbinding", this, [=](JNEvent &){
        d_treeView->unbindingChannels();
    });
    jnotify->on("monitor.toolbar.tree.channel.export", this, [=](JNEvent &){
        const QString filePath = QFileDialog::getSaveFileName(
                    this, tr("Save channel binding relations"), QString(), "XML file (*.xml)");
        if (filePath.isEmpty()) {
            return;
        }
        d_treeView->exportBindingStatus(filePath);
    });
    jnotify->on("monitor.toolbar.tree.channel.runAll", this, [=](JNEvent &){
        d_treeView->runAllChannels();
    });
    jnotify->on("monitor.toolbar.tree.channel.stopAll", this, [=](JNEvent &){
        d_treeView->stopAllChannels();
    });
}

TreeView::~TreeView()
{

}

bool TreeView::init()
{
    bool result = true;

    Json::Value option = JMain::instance()->option("monitor", "option/tree");
    if (option.isNull()) {
        return false;
    }
    // deep
    if (option.isMember("loadDeep")) {
        d_treeView->setLoadingDeep(option["loadDeep"].asInt());
    }
    // flush-period
    if (option.isMember("flushPeriod")) {
        d_treeView->setIntervalUpdate(option["flushPeriod"].asInt());
    }

    if (!updateParser()) {
        qDebug() << "monitor: update parser failure!";
        result = false;
    }

    return result;
}

void TreeView::setShowAttribute(int attr, bool on)
{
    d_treeView->setShowAttribute((Icd::CoreTreeWidget::ShowAttribute)attr, on);
}

void TreeView::setOrigValueRadix(int radix)
{
    if (radix < 2 || radix > 36) {
        d_treeView->setShowAttribute(Icd::CoreTreeWidget::ShowData, false);
    } else {
        d_treeView->setShowAttribute(Icd::CoreTreeWidget::ShowData, true);
        d_treeView->setDataFormat(radix);
    }
}

bool TreeView::updateParser()
{
    d_treeView->clearContents();

    const Json::Value config = JMain::instance()->option("monitor", "parser");
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
