#include "precomp.h"
#include "SimulateTreeView.h"

namespace Simulate {

TreeView::TreeView(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);
    vertLayoutMain->setSpacing(0);

    d_treeView = new Icd::CoreTreeWidget(this);
    d_treeView->setTreeMode(Icd::CoreTreeWidget::TreeModeSimulator);
    d_treeView->setBindTableType(Icd::CoreTreeWidget::BindOnlySend);
    vertLayoutMain->addWidget(d_treeView);

    connect(d_treeView, &Icd::CoreTreeWidget::itemPressed, this, [=](QStandardItem *item){
        jnotify->send("simulate.tree.item.pressed", qVariantFromValue((void*)item));
    });
    connect(d_treeView, &Icd::CoreTreeWidget::itemClicked, this, [=](QStandardItem *item){
        jnotify->send("simulate.tree.item.clicked", qVariantFromValue((void*)item));
    });
    connect(d_treeView, &Icd::CoreTreeWidget::currentItemChanged, this,
            [=](QStandardItem *current, QStandardItem *previous){
        QVariantList args;
        args.append(qVariantFromValue((void*)current));
        args.append(qVariantFromValue((void*)previous));
        jnotify->send("simulate.tree.item.currentchanged", args);
    });
    connect(d_treeView, &Icd::CoreTreeWidget::itemUnloaded, this,
            [=](QStandardItem *item, QStandardItem *tableItem){
        QVariantList args;
        args.append(qVariantFromValue((void*)item));
        args.append(qVariantFromValue((void*)tableItem));
        jnotify->send("simulate.tree.item.unloaded", args);
    });
    connect(d_treeView, &Icd::CoreTreeWidget::channelBound, this,
            [=](QStandardItem *item, const QString &channelId){
        QVariantList args;
        args.append(qVariantFromValue((void*)item));
        args.append(channelId);
        jnotify->send("simulate.tree.channel.bound", args);
    });
    connect(d_treeView, &Icd::CoreTreeWidget::channelUnbound, this,
            [=](QStandardItem *item, const QString &channelId){
        QVariantList args;
        args.append(qVariantFromValue((void*)item));
        args.append(channelId);
        jnotify->send("simulate.tree.channel.unbound", args);
    });
    connect(d_treeView, &Icd::CoreTreeWidget::channelChanged, this,
            [=](QStandardItem *item, const QString &channelId){
        QVariantList args;
        args.append(qVariantFromValue((void*)item));
        args.append(channelId);
        jnotify->send("simulate.tree.channel.changed", args);
    });
    connect(d_treeView, &Icd::CoreTreeWidget::unbindItem, this,
            [=](QStandardItem *item, QStandardItem *tableItem){
        QVariantList args;
        args.append(qVariantFromValue((void*)item));
        args.append(qVariantFromValue((void*)tableItem));
        jnotify->send("simulate.tree.item.unbind", args);
    });

    jnotify->on("simulate.parser.changed", this, [=](JNEvent &){
        if (!updateParser()) {
            qDebug() << "simulate: update parser failure!";
        }
    });
    jnotify->on("simulate.parser.inst", this, [=](JNEvent &event){
        Icd::JParserPtrHandle *handle = jVariantFromVoid<Icd::JParserPtrHandle>(event.argument());
        if (!handle) {
            return;
        }
        handle->parser = d_treeView->parser();
        event.setReturnValue(true);
    });

    jnotify->on("simulate.toolbar.database.config", this, [=](JNEvent &){
        QVariantList args;
        // module
        args.append("simulate");
        // receiver
        args.append(qVariantFromValue((void*)this));
        jnotify->send("database.config", args);
    });
    jnotify->on("simulate.toolbar.tree.loadDeep", this, [=](JNEvent &event){
        const int deep = event.argument().toInt();
        JMain::instance()->setOption("simulate", "option.tree.loadDeep", deep);
    });
    jnotify->on("simulate.toolbar.tree.showOffset", this, [=](JNEvent &event){
        const bool checked = event.argument().toBool();
        d_treeView->setShowAttribute(Icd::CoreTreeWidget::ShowOffset, checked);
        JMain::instance()->setOption("simulate", "option.tree.showOffset", checked);
    });
    jnotify->on("simulate.toolbar.tree.showType", this, [=](JNEvent &event){
        const bool checked = event.argument().toBool();
        d_treeView->setShowAttribute(Icd::CoreTreeWidget::ShowType, checked);
        JMain::instance()->setOption("simulate", "option.tree.showType", checked);
    });
    jnotify->on("simulate.toolbar.tree.showOrignal", this, [=](JNEvent &event){
        const QVariantList args = event.argument().toList();
        if (args.count() < 1) {
            return;
        }
        d_treeView->setShowAttribute(Icd::CoreTreeWidget::ShowData, args.at(0).toBool());
        if (args.count() < 2) {
            JMain::instance()->setOption("simulate", "option.tree.showOrignal", 0);
        } else {
            const int radix = args.at(1).toInt();
            d_treeView->setDataFormat(radix);
            JMain::instance()->setOption("simulate", "option.tree.showOrignal", radix);
        }
    });
    jnotify->on("simulate.toolbar.tree.showReal", this, [=](JNEvent &event){
        const bool checked = event.argument().toBool();
        d_treeView->setShowAttribute(Icd::CoreTreeWidget::ShowValue, checked);
        JMain::instance()->setOption("simulate", "option.tree.showValue", checked);
    });
    jnotify->on("simulate.toolbar.tree.showDesc", this, [=](JNEvent &event){
        const bool checked = event.argument().toBool();
        d_treeView->setShowAttribute(Icd::CoreTreeWidget::ShowSpec, checked);
        JMain::instance()->setOption("simulate", "option.tree.showDesc", checked);
    });
    jnotify->on("simulate.toolbar.tree.flushToggle", this, [=](JNEvent &event){
        const bool checked = event.argument().toBool();
        d_treeView->setRunning(checked);
        JMain::instance()->setOption("simulate", "option.tree.flushEnabled", checked);
    });
    jnotify->on("simulate.toolbar.tree.flushPeriod", this, [=](JNEvent &){
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
        JMain::instance()->setOption("simulate", "option.tree.flushPeriod", inputDlg.intValue());
    });
    jnotify->on("simulate.toolbar.tree.channel.binding", this, [=](JNEvent &){
        const QString filePath = QFileDialog::getOpenFileName(
                    this, tr("Select channel binding file"), QString(),
                    "XML file (*.xml)");
        if (filePath.isEmpty()) {
            return;
        }
        d_treeView->bindingChannels(filePath);
    });
    jnotify->on("simulate.toolbar.tree.channel.unbinding", this, [=](JNEvent &){
        d_treeView->unbindingChannels();
    });
    jnotify->on("simulate.toolbar.tree.channel.export", this, [=](JNEvent &){
        const QString filePath = QFileDialog::getSaveFileName(
                    this, tr("Save channel binding relations"), QString(),
                    "XML file (*.xml)");
        if (filePath.isEmpty()) {
            return;
        }
        d_treeView->exportBindingStatus(filePath);
    });
    jnotify->on("simulate.toolbar.tree.channel.runAll", this, [=](JNEvent &){
        d_treeView->runAllChannels();
    });
    jnotify->on("simulate.toolbar.tree.channel.stopAll", this, [=](JNEvent &){
        d_treeView->stopAllChannels();
    });
}

TreeView::~TreeView()
{

}

bool TreeView::init()
{
    bool result = true;

    Json::Value option = JMain::instance()->option("simulate", "option.tree");
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
        qDebug() << "simulate: update parser failure!";
        return false;
    }

    result = result && d_treeView->loadData();

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

    const Json::Value config = JMain::instance()->option("simulate", "parser");
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
