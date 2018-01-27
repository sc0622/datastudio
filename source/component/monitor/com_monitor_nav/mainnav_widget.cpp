#include "precomp.h"
#include "mainnav_widget.h"
#include "icdwidget/coretree_widget.h"

MainNavWidget::MainNavWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);

    d_icdTree = new Icd::CoreTreeWidget(this);
    d_icdTree->setTreeMode(Icd::CoreTreeWidget::TreeModeMonitor);
    vertLayoutMain->addWidget(d_icdTree);

    //
    connect(d_icdTree, &Icd::CoreTreeWidget::itemPressed, this, [=]
            (QStandardItem *item){
        //
        jframeNotifier().sendMessage("treeItemPressed@com_monitor", (JWPARAM)item);
    });
    connect(d_icdTree, &Icd::CoreTreeWidget::itemClicked, this, [=]
            (QStandardItem *item){
        //
        jframeNotifier().sendMessage("treeItemClick@com_monitor", (JWPARAM)item);
    });
    connect(d_icdTree, &Icd::CoreTreeWidget::currentItemChanged, this, [=]
            (QStandardItem *current, QStandardItem *previous){
        //
        jframeNotifier().sendMessage("currentItemChanged@com_monitor",
                                     (JWPARAM)current, (JLPARAM)previous);
    });
    connect(d_icdTree, &Icd::CoreTreeWidget::itemUnloaded, this, [=]
            (QStandardItem *item, QStandardItem *itemTable){
        //
        jframeNotifier().sendMessage("treeItemUnloaded@com_monitor",
                                     (JWPARAM)item, (JLPARAM)itemTable);
    });
    connect(d_icdTree, &Icd::CoreTreeWidget::channelBound, this, [=]
            (QStandardItem *item, const QString &channelId){
        //
        jframeNotifier().sendMessage("channelBound@com_monitor",
                                     (JWPARAM)item, (JLPARAM)&channelId);
    });
    connect(d_icdTree, &Icd::CoreTreeWidget::channelUnbound, this, [=]
            (QStandardItem *item, const QString &channelId){
        //
        jframeNotifier().sendMessage("channelUnbound@com_monitor",
                                     (JWPARAM)item, (JLPARAM)&channelId);
    });
    connect(d_icdTree, &Icd::CoreTreeWidget::channelChanged, this, [=]
            (QStandardItem *item, const QString &channelId){
        //
        jframeNotifier().sendMessage("channelChanged@com_monitor",
                                     (JWPARAM)item, (JLPARAM)&channelId);
    });
    connect(d_icdTree, &Icd::CoreTreeWidget::unbindItem, this, [=]
            (QStandardItem *item, QStandardItem *itemTable){
        //
        jframeNotifier().sendMessage("unbindItem@com_monitor",
                                     (JWPARAM)item, (JLPARAM)itemTable);
    });
}

bool MainNavWidget::init()
{
    bool result = true;

    //
    Icd::Parser *parser = 0;
    if (jframeNotifier().sendMessage("query_icd_parser@com_icdsystem",
                                     (JWPARAM)&parser) != JFrame::ResultSuccess) {
        return false;
    }
    d_icdTree->setParser(parser);

    // deep
    int deep = 0;
    if (jframeNotifier().sendMessage("query_loading_deep@com_monitor_nav",
                                     (JWPARAM)&deep) != JFrame::ResultSuccess) {
        return false;
    }
    setLoadingDeep(deep);

    //
    d_icdTree->setBindTableType(Icd::CoreTreeWidget::BindOnlyRecv);

    //
    result = result && d_icdTree->loadData();

    return result;
}

void MainNavWidget::releaseWork()
{

}

void MainNavWidget::setLoadingDeep(int deep)
{
    d_icdTree->setLoadingDeep(deep + Icd::ObjectVehicle);
}

void MainNavWidget::showItemOffset(bool checked)
{
    d_icdTree->setShowAttribute(Icd::CoreTreeWidget::ShowOffset, checked);
}

void MainNavWidget::showItemType(bool checked)
{
    d_icdTree->setShowAttribute(Icd::CoreTreeWidget::ShowType, checked);
}

void MainNavWidget::showItemData(bool checked)
{
    d_icdTree->setShowAttribute(Icd::CoreTreeWidget::ShowData, checked);
}

void MainNavWidget::setDataFormat(int format)
{
    d_icdTree->setDataFormat(format);
}

void MainNavWidget::showItemValue(bool checked)
{
    d_icdTree->setShowAttribute(Icd::CoreTreeWidget::ShowValue, checked);
}

void MainNavWidget::showItemSpec(bool checked)
{
    d_icdTree->setShowAttribute(Icd::CoreTreeWidget::ShowSpec, checked);
}

void MainNavWidget::setPlay(bool running)
{
    d_icdTree->setRunning(running);
    // notify com_monitor component
    if (jframeNotifier().sendMessage("setPlay@com_monitor",
                                     (JWPARAM)&running) != JFrame::ResultSuccess) {
        return;
    }
}

int MainNavWidget::intervalUpdate() const
{
    return d_icdTree->intervalUpdate();
}

void MainNavWidget::setIntervalUpdate(int interval)
{
    d_icdTree->setIntervalUpdate(interval);
}

void MainNavWidget::bindingChannels()
{
    const QString filePath = QFileDialog::getOpenFileName(
                this, QStringLiteral("选择通道绑定关系配置文件"),
                QString(),
                QStringLiteral("*.xml"));
    if (filePath.isEmpty()) {
        return;
    }

    d_icdTree->bindingChannels(filePath);
}

void MainNavWidget::unbindingChannels()
{
    d_icdTree->unbindingChannels();
}

void MainNavWidget::exportBindingStatus()
{
    const QString filePath = QFileDialog::getSaveFileName(
                this, QStringLiteral("保存通道绑定关系"),
                QString(),
                QStringLiteral("*.xml"));
    if (filePath.isEmpty()) {
        return;
    }

    d_icdTree->exportBindingStatus(filePath);
}

void MainNavWidget::runAllChannels()
{
    d_icdTree->runAllChannels();
}

void MainNavWidget::stopAllChannels()
{
    d_icdTree->stopAllChannels();
}

void MainNavWidget::updateUi()
{
    //
    d_icdTree->setParser(0);
    Icd::Parser *parser = 0;
    if (jframeNotifier().sendMessage("query_icd_parser@com_icdsystem",
                                     (JWPARAM)&parser) != JFrame::ResultSuccess) {
        return;
    }
    d_icdTree->setParser(parser);

    //
    if (!d_icdTree->loadData()) {
        //
    }
}
