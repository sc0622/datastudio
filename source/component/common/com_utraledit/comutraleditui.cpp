#include "precomp.h"
#include "comutraleditui.h"
#include "jutraledit/jutraledit_view.h"
#include "jutraledit_inc.h"
#include "icdwidget/coretree_widget.h"

ComUtralEditUi::ComUtralEditUi(IJAttempter &attempter, QWidget *parent)
    : QWidget(parent)
    , d_attempter(attempter)
    , d_item(0)
    , d_worker(Icd::WorkerPtr(0))
    , d_timerId(0)
{
    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);

    d_view = new JUtralEdit::JView(this);
    d_view->setAsciiVisible(false);
    vertLayoutMain->addWidget(d_view);

    //
    connect(Icd::WorkerPool::getInstance().get(), &Icd::WorkerPool::workerRemoved,
            this, &ComUtralEditUi::onWorkerRemoved);
    connect(Icd::WorkerPool::getInstance().get(), &Icd::WorkerPool::workerCleared,
            this, &ComUtralEditUi::onWorkerCleared);

    //
    d_view->setTitle(QStringLiteral("内存数据"));
}

ComUtralEditUi::~ComUtralEditUi()
{
    //
    setDirty();

    //
    Icd::WorkerPool::getInstance()->disconnect(this);
}

void ComUtralEditUi::setVisible(bool visible)
{
    QWidget::setVisible(visible);

    //
    updateView();
}

void ComUtralEditUi::start()
{
    if (d_timerId) {
        killTimer(d_timerId);
        d_timerId = 0;
    }

    d_timerId = startTimer(300);    // 300ms
}

void ComUtralEditUi::stop()
{
    if (d_timerId) {
        killTimer(d_timerId);
        d_timerId = 0;
    }
}

void ComUtralEditUi::onTreeItemClicked(QStandardItem *item)
{
    if (!item) {
        return;
    }

    //
    if (item == d_item) {
        return; // the same item
    }

    //
    const int itemType = item->type();
    if (itemType > Icd::TreeItemTypeTable) {
        return; //
    }

    //
    setDirty();

    //
    const QVariant varChannelId = item->data(Icd::TreeChannelIdRole);
    if (varChannelId.isNull()) {
        return; // invalid channel identity
    }

    //
    const QString channelId = varChannelId.toString();

    //
    const Icd::WorkerPtr worker =  Icd::WorkerPool::getInstance()
            ->workerByChannelIdentity(channelId.toStdString());
    if (!worker) {
        return; // invlaid worker
    }

    //
    d_item = item;
    //
    d_worker = worker;
    //
    d_view->setTitle(QStringLiteral("内存数据——") + QString::fromStdString(
                         d_worker->workerRecv()->table()->name()));
    //
    updateView();
    //
    if (d_worker->workerRecv()->isRunning()) {
        start();
    }
    //
    connect(d_worker->workerRecv().get(), &Icd::WorkerRecv::started,
            this, &ComUtralEditUi::onWorkerStarted);
    connect(d_worker->workerRecv().get(), &Icd::WorkerRecv::stopped,
            this, &ComUtralEditUi::onWorkerStopped);
}

void ComUtralEditUi::onCurrentItemChanged(QStandardItem *current, QStandardItem *previous)
{
    Q_UNUSED(previous);
    if (!current) {
        return;
    }
}

void ComUtralEditUi::onChannelUnbound(QStandardItem *item, const QString &channelId)
{
    if (!item) {
        return;
    }

    //
    const int itemType = item->type();
    if (itemType != Icd::TreeItemTypeTable) {
        return; //
    }

    //
    const Icd::WorkerPtr worker =  Icd::WorkerPool::getInstance()
            ->workerByChannelIdentity(channelId.toStdString());
    if (!worker) {
        return; // invlaid worker
    }
}

void ComUtralEditUi::onChannelChanged(QStandardItem *item, const QString &channelId)
{
    if (!item) {
        return;
    }

    //
    const int itemType = item->type();
    if (itemType != Icd::TreeItemTypeTable) {
        return; //
    }

    //
    const Icd::WorkerPtr worker =  Icd::WorkerPool::getInstance()
            ->workerByChannelIdentity(channelId.toStdString());
    if (!worker) {
        return; // invlaid worker
    }

    // change
    if (item == d_item) {
        //
        setDirty();
        //
        d_worker = worker;
        //
        d_view->setTitle(QStringLiteral("内存数据——") + QString::fromStdString(
                             d_worker->workerRecv()->table()->name()));
        //
        updateView();
        //
        if (worker->workerRecv()->isRunning()) {
            start();
        }
        //
        connect(d_worker->workerRecv().get(), &Icd::WorkerRecv::started,
                this, &ComUtralEditUi::onWorkerStarted);
        connect(d_worker->workerRecv().get(), &Icd::WorkerRecv::stopped,
                this, &ComUtralEditUi::onWorkerStopped);
    }
}

void ComUtralEditUi::onWorkerRemoved(const Icd::WorkerPtr &worker)
{
    if (worker == d_worker) {
        //
        setDirty();
    }
}

void ComUtralEditUi::onWorkerCleared()
{
    setDirty();
}

void ComUtralEditUi::onWorkerStarted()
{
    start();
}

void ComUtralEditUi::onWorkerStopped()
{
    stop();
}

void ComUtralEditUi::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == d_timerId) {
        updateView();
    } else {
        QWidget::timerEvent(event);
    }
}

void ComUtralEditUi::setDirty()
{
    //
    stop();

    if (d_worker) {
        d_worker->workerRecv()->disconnect(this);
    }

    //
    d_item = 0;
    //
    d_worker = Icd::WorkerPtr(0);
    //
    d_view->setTitle(QStringLiteral("内存数据"));
    //
    updateView();
}

void ComUtralEditUi::updateView()
{
    if (isHidden()) {
        return;     //
    }

    if (d_worker) {
        const Icd::TablePtr table = d_worker->workerRecv()->table();
        if (table) {
            d_view->updateUi(table->buffer(), table->bufferSize());
        } else {
            d_view->updateUi(QByteArray());
        }
    } else {
        d_view->updateUi(QByteArray());
    }
}

