#include "precomp.h"
#include "comchannelmgrui.h"
#include "module/comm/commgr_widget.h"

ComChannelMgrUi::ComChannelMgrUi(IJAttempter &attempter, QWidget *parent)
    : QWidget(parent)
    , d_attempter(attempter)
    , d_comgrWidget(0)
{
    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);

    d_comgrWidget = new ComMgrWidget(this);
    vertLayoutMain->addWidget(d_comgrWidget);

    //
    d_comgrWidget->updateUi();
}

ComChannelMgrUi::~ComChannelMgrUi()
{

}

void ComChannelMgrUi::releaseWork()
{
    // 停止所有工作组
    Icd::WorkerPool::getInstance()->stop();
}

