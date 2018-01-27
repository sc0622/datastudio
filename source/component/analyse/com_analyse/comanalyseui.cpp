#include "precomp.h"
#include "comanalyseui.h"
#include "mainview_widget.h"

ComAnalyseUi::ComAnalyseUi(IJAttempter &attempter, QWidget *parent)
    : QWidget(parent)
    , d_attempter(attempter)
{
    QHBoxLayout *horiLayoutMain = new QHBoxLayout(this);
    horiLayoutMain->setContentsMargins(0, 0, 0, 0);

    d_viewWidget = new MainViewWidget(this);
    horiLayoutMain->addWidget(d_viewWidget);
}

ComAnalyseUi::~ComAnalyseUi()
{

}

bool ComAnalyseUi::init()
{
    bool result = true;

    //
    result = result && d_viewWidget->init();

    return result;
}

void ComAnalyseUi::releaseWork()
{
    //
    d_viewWidget->releaseWork();
}

bool ComAnalyseUi::resetChart()
{
    return d_viewWidget->resetChart();
}

void ComAnalyseUi::onTreeItemClicked(QStandardItem *item)
{
    d_viewWidget->onTreeItemClicked(item);
}

void ComAnalyseUi::onTreeItemUnloaded(QStandardItem *item, QStandardItem *itemTable)
{
    d_viewWidget->onTreeItemUnloaded(item, itemTable);
}

void ComAnalyseUi::onUnbindItem(QStandardItem *item, QStandardItem *itemTable)
{
    d_viewWidget->onUnbindItem(item, itemTable);
}

void ComAnalyseUi::onTableLoaded(const QString &filePath, const Icd::TablePtr &table)
{
    d_viewWidget->onTableLoaded(filePath, table);
}

void ComAnalyseUi::setXAxisSync(bool enabled)
{
    d_viewWidget->setXAxisSync(enabled);
}

void ComAnalyseUi::showChartYLabel(bool checked)
{
    d_viewWidget->showChartYLabel(checked);
}

void ComAnalyseUi::showChartYAlign(bool align)
{
    d_viewWidget->showChartYAlign(align);
}

void ComAnalyseUi::setSyncTrack(bool enabled)
{
    d_viewWidget->setSyncTrack(enabled);
}

void ComAnalyseUi::setYLabelLength(int length)
{
    d_viewWidget->setYLabelLength(length);
}

void ComAnalyseUi::setColumnCount(int count)
{
    d_viewWidget->setColumnCount(count);
}

void ComAnalyseUi::updateData()
{
    d_viewWidget->updateData();
}

void ComAnalyseUi::toggleReplay(bool enabled)
{
    d_viewWidget->toggleReplay(enabled);
}

void ComAnalyseUi::clearCharts()
{
    d_viewWidget->clearCharts();
}

void ComAnalyseUi::setColumnWidthSame()
{
    d_viewWidget->setColumnWidthSame();
}

void ComAnalyseUi::setReplayPeriod(int interval)
{
    d_viewWidget->setReplayPeriod(interval);
}
