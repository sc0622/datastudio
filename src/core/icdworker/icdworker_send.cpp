#include "precomp.h"
#include "icdworker_send.h"
#include "private/icdworker_send_p.h"

namespace Icd {

WorkerSend::WorkerSend(QObject *parent)
    : QObject(parent)
    , J_DPTR(new WorkerSendPrivate(this))
{
    Q_D(WorkerSend);
    d->init();
}

WorkerSend::~WorkerSend()
{
    Q_D(WorkerSend);
    delete d;
}

ChannelPtr WorkerSend::channel() const
{
    Q_D(const WorkerSend);
    return d->channel;
}

void WorkerSend::setChannel(const ChannelPtr &channel)
{
    Q_D(WorkerSend);
    if (channel != d->channel) {
        d->mutex.lock();
        d->channel = channel;
        if (channel) {
            d->setInterval(channel->sendInterval());
            d->setTimeEvent(channel->autoSend() ? Icd::WorkerTrans::TimePeriodic
                                                : Icd::WorkerTrans::TimeOneShot);
        }
        d->mutex.unlock();
        emit channelChanged(d->channel);
        if (d->isRunning() && d->channel && !d->channel->isOpen()) {
            if (!d->channel->open()) {
                //
            }
        }
    }
}

TablePtr WorkerSend::table() const
{
    Q_D(const WorkerSend);
    return d->table;
}

void WorkerSend::setTable(const TablePtr &table)
{
    Q_D(WorkerSend);
    if (d->table != table) {
        d->mutex.lock();
        d->table = table;
        d->updateBind(table != Q_NULLPTR);
        d->mutex.unlock();
        emit tableChanged(d->table);
    }
}

int WorkerSend::interval() const
{
    Q_D(const WorkerSend);
    return d->interval();
}

void WorkerSend::setInterval(int value)
{
    Q_D(WorkerSend);
    if (value != d->interval()) {
        d->setInterval(value);
        emit intervalChanged(value);
    }
}

WorkerTrans::TimeEvent WorkerSend::timeEvent() const
{
    Q_D(const WorkerSend);
    return d->timeEvent();
}

void WorkerSend::setTimeEvent(WorkerTrans::TimeEvent value)
{
    Q_D(WorkerSend);
    if (value != d->timeEvent()) {
        d->setTimeEvent(value);
        if (value == WorkerTrans::TimeOneShot) {
            stop();
        }
        emit timeEventChanged(value);
    }
}

bool WorkerSend::counterLoop() const
{
    Q_D(const WorkerSend);
    QMutexLocker locker(const_cast<QMutex *>(&d->mutex));
    return d->counterLoop;
}

void WorkerSend::setCounterLoop(bool loop)
{
    Q_D(WorkerSend);
    if (loop != d->counterLoop) {
        d->counterLoop = loop;
        emit counterLoopChanged(loop);
    }
}

bool WorkerSend::frameLoop() const
{
    Q_D(const WorkerSend);
    QMutexLocker locker(const_cast<QMutex *>(&d->mutex));
    return d->frameLoop;
}

void WorkerSend::setFrameLoop(bool loop)
{
    Q_D(WorkerSend);
    if (loop != d->frameLoop) {
        d->frameLoop = loop;
        emit frameLoopChanged(loop);
    }
}

bool WorkerSend::isRunning() const
{
    Q_D(const WorkerSend);
    return d->channel && d->channel->isOpen() && d->isRunning();
}

bool WorkerSend::start()
{
    Q_D(WorkerSend);
    if (isRunning()) {
        return true;
    }

    if (!d->channel) {
        return false;
    }

    if (!d->channel->isOpen()) {
        return false;
    }

    if (!d->start()) {
        return false;
    }

    emit started();
    emit toggled(true);

    return true;
}

void WorkerSend::stop()
{
    Q_D(WorkerSend);
    if (!d->isRunning()) {
        return;
    }

    d->stop();

    emit stopped();
    emit toggled(false);
}

bool WorkerSend::sendOnce(bool counterLoop, bool frameLoop)
{
    Q_D(WorkerSend);
    if (!d->channel) {
        return false;
    }

    if (!d->channel->isOpen()) {
        return false;
    }

    if (!d->tableBuffer || !d->table->buffer()) {
        return false;
    }

    return d->sendData(counterLoop, frameLoop);
}

bool WorkerSend::sendOnce(const char *data, int size)
{
    if (!data || size < 0) {
        return false;
    }

    Q_D(WorkerSend);
    if (!d->channel) {
        return false;
    }

    if (!d->channel->isOpen()) {
        return false;
    }

    return d->channel->write(data, size);
}

} // end of namespace Icd
