#include "precomp.h"
#include "mainview_widget.h"
#include "icdchart/icdchart_view.h"
#include "icdchart/icdchart_fileview.h"
#include "icdparser/icdparser.h"
#include "icdwidget/coretree_widget.h"
#include "icdwidget/progressdialog.h"
#include "icdworker/icdworker_pool.h"
#include "icdcore/icd_table.h"
#include "icdcore/icd_item.h"
#include "icdcore/icd_item_bit.h"

MainViewWidget::MainViewWidget(QWidget *parent)
    : QFrame(parent)
{
    setObjectName("mainView");
    QHBoxLayout *horiLayoutMain = new QHBoxLayout(this);
    horiLayoutMain->setContentsMargins(0, 0, 0, 0);

    d_chartView = new  Icd::ChartFileView(this);
    horiLayoutMain->addWidget(d_chartView);

    //
    connect(d_chartView, &Icd::ChartFileView::replayFinished, this, [=](){
        jframeNotifier().postMessage("setReplayState@com_analuse", (JWPARAM)false);
    });
}

bool MainViewWidget::init()
{
    bool result = true;

    //

    return result;
}

void MainViewWidget::releaseWork()
{
    //
    resetChart();
}

bool MainViewWidget::resetChart()
{
    d_chartView->reset();

    return true;
}

void MainViewWidget::onTreeItemClicked(QStandardItem *item)
{
    if (!item) {
        return;
    }

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
    if (!item) {
        return;
    }

    //
    // get type of treeWidgetItem
    const Icd::TreeItemType itemType = (Icd::TreeItemType)item->type();
    QString domain = item->data(Icd::TreeItemDomainRole).toString();
    if (itemTable) {
        const QString filePath = itemTable->data(Icd::TreeFilePathRole).toString();
        domain = filePath + '/' + domain;
    }

    // dispatch
    switch (itemType) {
    case Icd::TreeItemTypeRoot:
        break;
    case Icd::TreeItemTypeVehicle:
    case Icd::TreeItemTypeSystem:
    case Icd::TreeItemTypeTable:
    case Icd::TreeItemTypeDataItem:
        //
        d_chartView->removeItem(domain);
        //
        if (itemType == Icd::TreeItemTypeTable) {
            const QString filePath = item->data(Icd::TreeFilePathRole).toString();
            d_chartView->removeTable(filePath);
        }
        break;
    default:
        break;
    }
}

void MainViewWidget::onUnbindItem(QStandardItem *item, QStandardItem *itemTable)
{
    if (!item) {
        return;
    }

    //
    QString domain = item->data(Icd::TreeItemDomainRole).toString();
    if (itemTable) {
        const QString filePath = itemTable->data(Icd::TreeFilePathRole).toString();
        domain = filePath + '/' + domain;
        d_chartView->removeItem(domain);
    } else {
        int rowCount = item->rowCount();
        for (int row = 0; row < rowCount; ++row) {
            QStandardItem *itemChild = item->child(row);
            if (itemChild && itemChild->type() == Icd::TreeItemTypeTable) {
                const QString filePath = itemChild->data(Icd::TreeFilePathRole).toString();
                d_chartView->removeItem(filePath + '/' + domain);
                itemChild->setData(QVariant::Invalid, Icd::TreeBoundRole);
            }
        }
    }

    //
    item->setData(QVariant::Invalid, Icd::TreeBoundRole);
}

void MainViewWidget::onTableLoaded(const QString &filePath, const Icd::TablePtr &table)
{
    d_chartView->addTable(filePath, table);
}

void MainViewWidget::setXAxisSync(bool enabled)
{
    d_chartView->setXAxisSync(enabled);
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

void MainViewWidget::updateData()
{
    d_chartView->updateData();
}

void MainViewWidget::toggleReplay(bool enabled)
{
    d_chartView->toggleReplay(enabled);
}

void MainViewWidget::setColumnCount(int count)
{
    d_chartView->setColumnCount(count);
}

void MainViewWidget::clearCharts()
{
    d_chartView->clearCharts();
}

void MainViewWidget::setColumnWidthSame()
{
    d_chartView->setColumnWidthSame();
}

void MainViewWidget::setReplayPeriod(int interval)
{
    d_chartView->setReplayPeriod(interval);
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
    painter.drawText(rect(), QStringLiteral("<拖动[数据分析]系统左边树节点到此视图>"), textOption);

    painter.restore();

    QFrame::paintEvent(event);
}
