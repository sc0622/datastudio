#include "precomp.h"
#include "commonitorui.h"
#include "mainview_widget.h"

ComMonitorUi::ComMonitorUi(IJAttempter &attempter, QWidget *parent)
    : QWidget(parent)
    , d_attempter(attempter)
{
    QHBoxLayout *horiLayoutMain = new QHBoxLayout(this);
    horiLayoutMain->setContentsMargins(0, 0, 0, 0);

    d_viewWidget = new MainViewWidget(this);
    horiLayoutMain->addWidget(d_viewWidget);
}

ComMonitorUi::~ComMonitorUi()
{

}

bool ComMonitorUi::init()
{
    bool result = true;

    //
    result = result && d_viewWidget->init();

    return result;
}

bool ComMonitorUi::onChartOrder()
{
    return d_viewWidget->onChartOrder();
}

void ComMonitorUi::releaseWork()
{
    d_viewWidget->releaseWork();
}

void ComMonitorUi::clearCharts()
{
    d_viewWidget->clearCharts();
}

void ComMonitorUi::setColumnWidthSame()
{
    d_viewWidget->setColumnWidthSame();
}

void ComMonitorUi::showChartYLabel(bool checked)
{
    d_viewWidget->showChartYLabel(checked);
}

void ComMonitorUi::showChartYAlign(bool align)
{
    d_viewWidget->showChartYAlign(align);
}

void ComMonitorUi::setSyncTrack(bool enabled)
{
    d_viewWidget->setSyncTrack(enabled);
}

void ComMonitorUi::setYLabelLength(int length)
{
    d_viewWidget->setYLabelLength(length);
}

void ComMonitorUi::setColumnCount(int count)
{
    d_viewWidget->setColumnCount(count);
}

void ComMonitorUi::onTreeItemClicked(QStandardItem *item)
{
    d_viewWidget->onTreeItemClicked(item);
}

void ComMonitorUi::onTreeItemUnloaded(QStandardItem *item, QStandardItem *itemTable)
{
    d_viewWidget->onTreeItemUnloaded(item, itemTable);
}

void ComMonitorUi::onChannelUnbound(QStandardItem *item, const QString &channelId)
{
    d_viewWidget->onChannelUnbound(item, channelId);
}

void ComMonitorUi::onChannelChanged(QStandardItem *item, const QString &channelId)
{
    d_viewWidget->onChannelChanged(item, channelId);
}

void ComMonitorUi::onUnbindItem(QStandardItem *item, QStandardItem *itemTable)
{
    d_viewWidget->onUnbindItem(item, itemTable);
}

void ComMonitorUi::setPlay(bool running)
{
    d_viewWidget->setPlay(running);
}

