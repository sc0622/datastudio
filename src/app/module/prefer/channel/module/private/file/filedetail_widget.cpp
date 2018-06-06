#include "precomp.h"
#include "filedetail_widget.h"
#include "filesetting_widget.h"
#include "filesend_widget.h"
#include "filerecv_widget.h"
#include "icdworker/icdworker_pool.h"

FileDetailWidget::FileDetailWidget(QWidget *parent)
    : QWidget(parent)
    , d_worker(Icd::WorkerPtr(0))
{
    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);

    d_settingWidget = new FileSettingWidget(this);
    vertLayoutMain->addWidget(d_settingWidget);

    d_sendWidget = new FileSendWidget(this);
    vertLayoutMain->addWidget(d_sendWidget);

    d_recvWidget = new FileRecvWidget(this);
    vertLayoutMain->addWidget(d_recvWidget);

    vertLayoutMain->addStretch();

    //
    connect(Icd::WorkerPool::getInstance().get(), &Icd::WorkerPool::workerRemoved,
            this, &FileDetailWidget::onWorkerRemoved);
    connect(Icd::WorkerPool::getInstance().get(), &Icd::WorkerPool::workerCleared,
            this, &FileDetailWidget::onWorkerCleared);
}

FileDetailWidget::~FileDetailWidget()
{
    //
    Icd::WorkerPool::getInstance()->disconnect(this);
}

void FileDetailWidget::updateUi(const Icd::WorkerPtr &worker)
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

void FileDetailWidget::onWorkerRemoved(const Icd::WorkerPtr &worker)
{
    if (worker != d_worker) {
        return;
    }

    //
    d_worker = Icd::WorkerPtr(0);

    //
    updateUi(d_worker);
}

void FileDetailWidget::onWorkerCleared()
{
    //
    d_worker = Icd::WorkerPtr(0);

    //
    updateUi(d_worker);
}
