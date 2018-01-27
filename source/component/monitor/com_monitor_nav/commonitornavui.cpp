#include "precomp.h"
#include "commonitornavui.h"
#include "mainnav_widget.h"

ComMonitorNavUi::ComMonitorNavUi(IJAttempter &attempter, QWidget *parent)
    : QWidget(parent)
    , d_attempter(attempter)
{
    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);

    d_navWidget = new MainNavWidget(this);
    vertLayoutMain->addWidget(d_navWidget);
}

ComMonitorNavUi::~ComMonitorNavUi()
{

}

bool ComMonitorNavUi::init()
{
    bool result = true;

    //
    result = result && d_navWidget->init();

    return result;
}

void ComMonitorNavUi::releaseWork()
{
    //
    d_navWidget->releaseWork();
}

void ComMonitorNavUi::setLoadingDeep(int deep)
{
    d_navWidget->setLoadingDeep(deep);
}

void ComMonitorNavUi::showItemOffset(bool checked)
{
    d_navWidget->showItemOffset(checked);
}

void ComMonitorNavUi::showItemType(bool checked)
{
    d_navWidget->showItemType(checked);
}

void ComMonitorNavUi::showItemData(bool checked)
{
    d_navWidget->showItemData(checked);
}

void ComMonitorNavUi::setDataFormat(int format)
{
    d_navWidget->setDataFormat(format);
}

void ComMonitorNavUi::showItemValue(bool checked)
{
    d_navWidget->showItemValue(checked);
}

void ComMonitorNavUi::showItemSpec(bool checked)
{
    d_navWidget->showItemSpec(checked);
}

void ComMonitorNavUi::setPlay(bool running)
{
    d_navWidget->setPlay(running);
}

int ComMonitorNavUi::intervalUpdate() const
{
    return d_navWidget->intervalUpdate();
}

void ComMonitorNavUi::setIntervalUpdate(int interval)
{
    d_navWidget->setIntervalUpdate(interval);
}

void ComMonitorNavUi::bindingChannels()
{
    d_navWidget->bindingChannels();
}

void ComMonitorNavUi::unbindingChannels()
{
    d_navWidget->unbindingChannels();
}

void ComMonitorNavUi::exportBindingStatus()
{
    d_navWidget->exportBindingStatus();
}

void ComMonitorNavUi::runAllChannels()
{
    d_navWidget->runAllChannels();
}

void ComMonitorNavUi::stopAllChannels()
{
    d_navWidget->stopAllChannels();
}

void ComMonitorNavUi::updateUi()
{
    d_navWidget->updateUi();
}
