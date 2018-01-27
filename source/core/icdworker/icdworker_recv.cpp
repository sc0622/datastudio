#include "precomp.h"
#include "icdworker_recv.h"
#include "private/icdworker_recv_p.h"
#include "icdworker.h"

namespace Icd {

WorkerRecv::WorkerRecv(QObject *parent)
    : QObject(parent)
    , d_ptr(new WorkerRecvPrivate(this))
{
    Q_D(WorkerRecv);
    d->init();
}

WorkerRecv::~WorkerRecv()
{
    Q_D(WorkerRecv);
    delete d;
}

ChannelPtr WorkerRecv::channel() const
{
    Q_D(const WorkerRecv);
    return d->channel;
}

void WorkerRecv::setChannel(const ChannelPtr &channel)
{
    Q_D(WorkerRecv);
    if (channel != d->channel) {
        d->mutex.lock();
        d->channel = channel;
        d->mutex.unlock();
        emit channelChanged(d->channel);
        if (d->isRunning() && d->channel && !d->channel->isOpen()) {
            if (!d->channel->open()) {
                //
            }
        }
    }
}

Icd::TablePtr WorkerRecv::table() const
{
    Q_D(const WorkerRecv);
    return d->table;
}

void WorkerRecv::setTable(const Icd::TablePtr &table)
{
    Q_D(WorkerRecv);
    if (d->table != table) {
        d->mutex.lock();
        d->table = table;
        d->updateBind(table != Q_NULLPTR);
        d->mutex.unlock();
        emit tableChanged(d->table);
    }
}

Icd::WorkerPtr WorkerRecv::relayer() const
{
    Q_D(const WorkerRecv);
    return d->relayer;
}

void WorkerRecv::setRelayer(const Icd::WorkerPtr &relayer)
{
    Q_D(WorkerRecv);
    if (!d->channel) {
        return;
    }
    //
    if (relayer != d->relayer) {
        d->mutex.lock();
        d->relayer = relayer;
        if (relayer) {
            d->channel->setRelayer(relayer->channel());
        } else {
            d->channel->setRelayer(ChannelPtr());
        }
        d->mutex.unlock();
        emit relayerChanged();
    }
}

int WorkerRecv::interval() const
{
    Q_D(const WorkerRecv);
    return d->interval();
}

void WorkerRecv::setInterval(int value)
{
    Q_D(WorkerRecv);
    if (value != d->interval()) {
        d->setInterval(value);
        emit intervalChanged(value);
    }
}

WorkerTrans::TimeEvent WorkerRecv::timeEvent() const
{
    Q_D(const WorkerRecv);
    return d->timeEvent();
}

void WorkerRecv::setTimeEvent(WorkerTrans::TimeEvent value)
{
    Q_D(WorkerRecv);
    if (value != d->timeEvent()) {
        d->setTimeEvent(value);
        emit timeEventChanged(value);
    }
}

bool WorkerRecv::isRunning() const
{
    Q_D(const WorkerRecv);
    return d->channel && d->channel->isOpen() && d->isRunning();
}

bool WorkerRecv::start()
{
    Q_D(WorkerRecv);
    if (isRunning()) {
        return true;
    }

    if (!d->channel) {
        return false;
    }

    if (!d->channel->isOpen()) {
        return false;
    }

    d->mutex.lock();
    d->updateBind(true);
    d->mutex.unlock();

    if (d->relayer) {
        d->relayer->open();
    }

    if (!d->start()) {
        return false;
    }

    emit started();
    emit toggled(true);

    return true;
}

void WorkerRecv::stop()
{
    Q_D(WorkerRecv);
    if (!d->isRunning()) {
        return;
    }

    d->stopRecord();
    d->stop();

    if (d->relayer) {
        d->relayer->close();
    }

    emit stopped();
    emit toggled(false);
}

int WorkerRecv::queueCount() const
{
    Q_D(const WorkerRecv);
    return d->queueCount;
}

bool WorkerRecv::startRecord()
{
    Q_D(WorkerRecv);
    return d->startRecord();
}

void WorkerRecv::stopRecord()
{
    Q_D(WorkerRecv);
    d->stopRecord();
}

bool WorkerRecv::isRecording() const
{
    Q_D(const WorkerRecv);
    return d->recordFile.isOpen();
}

void WorkerRecv::setQueueCount(int count)
{
    Q_D(WorkerRecv);
    d->mutex.lock();
    d->queueCount = count;
    while (d->bufferQueue.count() > d->queueCount) {
        char *buffer = d->bufferQueue.dequeue();
        delete[] buffer;
    }
    d->mutex.unlock();
}

} // end of namespace Icd
