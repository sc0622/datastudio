#include "precomp.h"
#include "serialdetail_widget.h"
#include "serialsetting_widget.h"
#include "serialsend_widget.h"
#include "serialrecv_widget.h"
#include "icdworker/icdworker_pool.h"

SerialDetailWidget::SerialDetailWidget(QWidget *parent)
    : QWidget(parent)
    , d_worker(Icd::WorkerPtr(0))
{
    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);

    d_settingWidget = new SerialSettingWidget(this);
    vertLayoutMain->addWidget(d_settingWidget);

    d_sendWidget = new SerialSendWidget(this);
    vertLayoutMain->addWidget(d_sendWidget);

    d_recvWidget = new SerialRecvWidget(this);
    vertLayoutMain->addWidget(d_recvWidget);

    vertLayoutMain->addStretch();

    //
    connect(Icd::WorkerPool::getInstance().get(), &Icd::WorkerPool::workerRemoved,
            this, &SerialDetailWidget::onWorkerRemoved);
    connect(Icd::WorkerPool::getInstance().get(), &Icd::WorkerPool::workerCleared,
            this, &SerialDetailWidget::onWorkerCleared);
}

SerialDetailWidget::~SerialDetailWidget()
{
    //
    Icd::WorkerPool::getInstance()->disconnect(this);
}

void SerialDetailWidget::updateUi(const Icd::WorkerPtr &worker)
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

void SerialDetailWidget::onWorkerRemoved(const Icd::WorkerPtr &worker)
{
    if (worker != d_worker) {
        return;
    }

    //
    d_worker = Icd::WorkerPtr(0);

    //
    updateUi(d_worker);
}

void SerialDetailWidget::onWorkerCleared()
{
    //
    d_worker = Icd::WorkerPtr(0);

    //
    updateUi(d_worker);
}
