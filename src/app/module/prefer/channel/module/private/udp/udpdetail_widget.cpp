#include "precomp.h"
#include "udpdetail_widget.h"
#include "udpsetting_widget.h"
#include "udpsend_widget.h"
#include "udprecv_widget.h"
#include "icdworker/icdworker_pool.h"

UdpDetailWidget::UdpDetailWidget(QWidget *parent)
    : QWidget(parent)
    , d_worker(Icd::WorkerPtr(0))
{
    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);

    d_settingWidget = new UdpSettingWidget(this);
    vertLayoutMain->addWidget(d_settingWidget);

    d_sendWidget = new UdpSendWidget(this);
    vertLayoutMain->addWidget(d_sendWidget);

    d_recvWidget = new UdpRecvWidget(this);
    vertLayoutMain->addWidget(d_recvWidget);

    vertLayoutMain->addStretch();

    //
    connect(Icd::WorkerPool::getInstance().get(), &Icd::WorkerPool::workerRemoved,
            this, &UdpDetailWidget::onWorkerRemoved);
    connect(Icd::WorkerPool::getInstance().get(), &Icd::WorkerPool::workerCleared,
            this, &UdpDetailWidget::onWorkerCleared);
}

UdpDetailWidget::~UdpDetailWidget()
{
    //
    Icd::WorkerPool::getInstance()->disconnect(this);
}

void UdpDetailWidget::updateUi(const Icd::WorkerPtr &worker)
{
    //
    d_worker = worker;

    //
    d_settingWidget->updateUi(worker);

    //
    d_sendWidget->updateUi(worker);

    //
    d_recvWidget->updateUi(worker);
}

void UdpDetailWidget::onWorkerRemoved(const Icd::WorkerPtr &worker)
{
    if (worker != d_worker) {
        return;
    }

    //
    d_worker = Icd::WorkerPtr(0);

    //
    updateUi(d_worker);
}

void UdpDetailWidget::onWorkerCleared()
{
    //
    d_worker = Icd::WorkerPtr(0);

    //
    updateUi(d_worker);
}
