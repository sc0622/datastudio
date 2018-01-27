#include "precomp.h"
#include "comsimulatornavui.h"
#include "mainnav_widget.h"

ComSimulatorNavUi::ComSimulatorNavUi(IJAttempter &attempter, QWidget *parent)
    : QWidget(parent)
    , d_attempter(attempter)
{
    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);

    d_navWidget = new MainNavWidget(this);
    vertLayoutMain->addWidget(d_navWidget);
}

ComSimulatorNavUi::~ComSimulatorNavUi()
{

}

bool ComSimulatorNavUi::init()
{
    bool result = true;

    //
    result = result && d_navWidget->init();

    return result;
}

void ComSimulatorNavUi::releaseWork()
{
    //
    d_navWidget->releaseWork();
}

void ComSimulatorNavUi::setLoadingDeep(int deep)
{
    d_navWidget->setLoadingDeep(deep);
}

void ComSimulatorNavUi::showItemOffset(bool checked)
{
    d_navWidget->showItemOffset(checked);
}

void ComSimulatorNavUi::showItemType(bool checked)
{
    d_navWidget->showItemType(checked);
}

void ComSimulatorNavUi::showItemData(bool checked)
{
    d_navWidget->showItemData(checked);
}

void ComSimulatorNavUi::setDataFormat(int format)
{
    d_navWidget->setDataFormat(format);
}

void ComSimulatorNavUi::showItemValue(bool checked)
{
    d_navWidget->showItemValue(checked);
}

void ComSimulatorNavUi::showItemSpec(bool checked)
{
    d_navWidget->showItemSpec(checked);
}

void ComSimulatorNavUi::setPlay(bool running)
{
    d_navWidget->setPlay(running);
}

int ComSimulatorNavUi::intervalUpdate() const
{
    return d_navWidget->intervalUpdate();
}

void ComSimulatorNavUi::setIntervalUpdate(int interval)
{
    d_navWidget->setIntervalUpdate(interval);
}

void ComSimulatorNavUi::bindingChannels()
{
    d_navWidget->bindingChannels();
}

void ComSimulatorNavUi::unbindingChannels()
{
    d_navWidget->unbindingChannels();
}

void ComSimulatorNavUi::exportBindingStatus()
{
    d_navWidget->exportBindingStatus();
}

void ComSimulatorNavUi::runAllChannels()
{
    d_navWidget->runAllChannels();
}

void ComSimulatorNavUi::stopAllChannels()
{
    d_navWidget->stopAllChannels();
}

void ComSimulatorNavUi::updateUi()
{
    d_navWidget->updateUi();
}
