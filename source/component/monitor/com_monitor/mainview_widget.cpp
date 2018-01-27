#include "precomp.h"
#include "mainview_widget.h"
#include "module/chart_order_widget.h"
#include "icdchart/icdchart_view.h"
#include "icdwidget/coretree_widget.h"
#include "icdworker/icdworker_pool.h"

MainViewWidget::MainViewWidget(QWidget *parent)
    : QFrame(parent)
{
    setObjectName("mainView");
    QHBoxLayout *horiLayoutMain = new QHBoxLayout(this);
    horiLayoutMain->setContentsMargins(0, 0, 0, 0);

    d_chartView = new  Icd::ChartView(this);
    horiLayoutMain->addWidget(d_chartView);

    //
    QFile file(":/icdsystem/com_monitor/qss/default.qss");
    if (file.open(QFile::ReadOnly)) {
        setStyleSheet(file.readAll());
    }
#if 1
    QFileSystemWatcher *fileWatcher = new QFileSystemWatcher(this);
    fileWatcher->addPath(QString::fromStdString(jframeFacade()->thisDirPath())
                         .append("/source/component/monitor/com_monitor/resource/qss/default.qss"));
    connect(fileWatcher, &QFileSystemWatcher::fileChanged, [=](const QString &path){
        QFile file(path);
        if (file.open(QFile::ReadOnly)) {
            setStyleSheet(file.readAll());
        }
    });
#endif
}

bool MainViewWidget::init()
{
    bool result = true;

    //

    return result;
}

bool MainViewWidget::onChartOrder()
{
    //
    ChartOrderWidget chartOrderWidget(this);

    //

    //
    if (chartOrderWidget.exec() != QDialog::Accepted) {
        return false;
    }

    //

    return true;
}

void MainViewWidget::releaseWork()
{
    //
    d_chartView->setRunning(false);
}

void MainViewWidget::clearCharts()
{
    d_chartView->clearCharts();
}

void MainViewWidget::setColumnWidthSame()
{
    d_chartView->setColumnWidthSame();
}

void MainViewWidget::showChartYLabel(bool checked)
{
    d_chartView->showYLabel(checked);
}

void MainViewWidget::showChartYAlign(bool align)
{
    d_chartView->showYAlign(align);
}

void MainViewWidget::setSyncTrack(bool enabled)
{
    d_chartView->setSyncTrack(enabled);
}

void MainViewWidget::setYLabelLength(int length)
{
    d_chartView->setYLabelLength(length);
}

void MainViewWidget::setColumnCount(int count)
{
    d_chartView->setColumnCount(count);
}

void MainViewWidget::onTreeItemClicked(QStandardItem *item)
{
    if (!item) {
        return;
    }

    //
    // get type of treeWidgetItem
    const Icd::TreeItemType itemType = (Icd::TreeItemType)item->type();
    const QString domain = item->data(Icd::TreeItemDomainRole).toString();

    // dispatch
    switch (itemType) {
    case Icd::TreeItemTypeRoot:
        break;
    case Icd::TreeItemTypeVehicle:
    case Icd::TreeItemTypeSystem:
    case Icd::TreeItemTypeTable:
    case Icd::TreeItemTypeDataItem:
    case Icd::TreeItemTypeItemTable:
        d_chartView->focusItem(domain);
        break;
    default:
        break;
    }
}

void MainViewWidget::onTreeItemUnloaded(QStandardItem *item, QStandardItem *itemTable)
{
    Q_UNUSED(itemTable);
    if (!item) {
        return;
    }

    //
    // get type of treeWidgetItem
    const Icd::TreeItemType itemType = (Icd::TreeItemType)item->type();
    const QString domain = item->data(Icd::TreeItemDomainRole).toString();

    // dispatch
    switch (itemType) {
    case Icd::TreeItemTypeRoot:
        break;
    case Icd::TreeItemTypeVehicle:
    case Icd::TreeItemTypeSystem:
    case Icd::TreeItemTypeTable:
    case Icd::TreeItemTypeDataItem:
        d_chartView->removeItem(domain);
        break;
    default:
        break;
    }
}

void MainViewWidget::onChannelUnbound(QStandardItem *item, const QString &channelId)
{
    Q_UNUSED(channelId);
    if (!item) {
        return;
    }

    //
    const QString domain = item->data(Icd::TreeItemDomainRole).toString();

    //
    d_chartView->removeItem(domain);
}

void MainViewWidget::onChannelChanged(QStandardItem *item, const QString &channelId)
{
    Q_UNUSED(channelId);
    if (!item) {
        return;
    }

    //
    const Icd::WorkerPtr worker = Icd::WorkerPool::getInstance()
            ->workerByChannelIdentity(channelId.toStdString());
    if (!worker) {
        return;
    }

    //
    const QString domain = item->data(Icd::TreeItemDomainRole).toString();

    //
    d_chartView->changeWorker(domain, worker);
}

void MainViewWidget::onUnbindItem(QStandardItem *item, QStandardItem *itemTable)
{
    Q_UNUSED(itemTable);
    if (!item) {
        return;
    }
    //
    QString domain = item->data(Icd::TreeItemDomainRole).toString();
    //
    d_chartView->removeItem(domain);
    //
    item->setData(QVariant::Invalid, Icd::TreeBoundRole);
}

void MainViewWidget::setPlay(bool running)
{
    d_chartView->setRunning(running);
}

void MainViewWidget::paintEvent(QPaintEvent *event)
{
    //
    QPainter painter(this);
    painter.save();

    QFont font("microsoft yahei", 24);
    painter.setFont(font);
    painter.setPen(QPen(QColor(64, 64, 64, 80)));
    QTextOption textOption(Qt::AlignCenter);
    painter.drawText(rect(), QStringLiteral("<拖动[监控系统]左边树节点到此视图>"), textOption);

    painter.restore();

    QFrame::paintEvent(event);
}

