#include "precomp.h"
#include "comsimulatorui.h"
#include "mainview_widget.h"

ComSimulatorUi::ComSimulatorUi(IJAttempter &attempter, QWidget *parent)
    : QMainWindow(parent)
    , d_attempter(attempter)
{
    d_centralWidget = new QWidget(this);
    setCentralWidget(d_centralWidget);

    QVBoxLayout *vertLayoutMain = new QVBoxLayout(d_centralWidget);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);

    d_viewWidget = new MainViewWidget(this);
    vertLayoutMain->addWidget(d_viewWidget);
}

ComSimulatorUi::~ComSimulatorUi()
{

}

void ComSimulatorUi::releaseWork()
{

}

void ComSimulatorUi::onTreeItemClicked(QStandardItem *item)
{
    d_viewWidget->onTreeItemClicked(item);
}

void ComSimulatorUi::onTreeItemUnloaded(QStandardItem *item, QStandardItem *itemTable)
{
    d_viewWidget->onTreeItemUnloaded(item, itemTable);
}

void ComSimulatorUi::onChannelUnbound(QStandardItem *item, const QString &channelId)
{
    d_viewWidget->onChannelUnbound(item, channelId);
}

void ComSimulatorUi::onChannelChanged(QStandardItem *item, const QString &channelId)
{
    d_viewWidget->onChannelChanged(item, channelId);
}

void ComSimulatorUi::onUnbindItem(QStandardItem *item)
{
    d_viewWidget->onUnbindItem(item);
}

