#include "precomp.h"
#include "icdworker.h"
#include "icdworker_send.h"
#include "icdworker_recv.h"
#include "icdcomm/icdcomm_filechannel.h"
#include <QFileInfo>

namespace Icd {

/**
 * @brief _setChannelFilePath
 * @param table
 */
void _updateRelayer(Icd::ChannelPtr &channel, const Icd::TablePtr &table)
{
    if (!channel || !table) {
        return;
    }

    Icd::ChannelPtr relayer = channel->relayer();
    if (!relayer || relayer->channelType() != Icd::ChannelFile) {
        return;
    }

    //
    Icd::FileChannelPtr fileRelayer = JHandlePtrCast<Icd::FileChannel, Icd::Channel>(relayer);
    if (!fileRelayer) {
        return;
    }

    // domain
    fileRelayer->setDomain(table->domain());

    //
    QFileInfo fileInfo(QString::fromStdString(fileRelayer->filePath()));
    fileRelayer->setFilePath(fileInfo.path().toStdString().append("/")
                             .append(table->name()).append("-")
                             .append(QDateTime::currentDateTime()
                                     .toString("yyyyMMddhhmmsszzz").toStdString())
                             .append(".icd"));
}

// WorkerPrivate

class WorkerPrivate
{
    friend class Worker;
public:
    WorkerPrivate(Worker *q)
        : q_ptr(q)
        , channel(0)
        , workerSend(0)
        , workerRecv(0)
    {

    }

    void init();

private:
    J_DECLARE_PUBLIC(Worker)
    Icd::ChannelPtr channel;
    WorkerSendPtr workerSend;
    WorkerRecvPtr workerRecv;
};

void WorkerPrivate::init()
{
    Q_Q(Worker);

    //
    workerSend = WorkerSendPtr(new WorkerSend(q));
    workerRecv = WorkerRecvPtr(new WorkerRecv(q));

    //
    QObject::connect(workerSend.get(), &Icd::WorkerSend::closed, q, &Icd::Worker::closed);
    QObject::connect(workerRecv.get(), &Icd::WorkerRecv::closed, q, &Icd::Worker::closed);
}

// class Worker

Worker::Worker(QObject *parent)
    : QObject(parent)
    , d_ptr(new WorkerPrivate(this))
{
    Q_D(Worker);
    d->init();
}

Worker::Worker(const Icd::ChannelPtr &channel, QObject *parent)
    : QObject(parent)
    , d_ptr(new WorkerPrivate(this))
{
    Q_D(Worker);
    d->init();

    //
    d->channel = channel;
    d->workerSend->setChannel(channel);
    d->workerRecv->setChannel(channel);
}

Worker::Worker(const Icd::ChannelPtr &channel, const Icd::TablePtr &tableSend,
               const Icd::TablePtr &tableRecv, QObject *parent)
    : QObject(parent)
    , d_ptr(new WorkerPrivate(this))
{
    Q_D(Worker);
    d->init();

    //
    d->channel = channel;
    d->workerSend->setChannel(channel);
    d->workerRecv->setChannel(channel);
    d->workerSend->setTable(tableSend);
    d->workerRecv->setTable(tableRecv);
}

Worker::~Worker()
{
    Q_D(Worker);
    delete d;
}

Icd::ChannelPtr Worker::channel() const
{
    Q_D(const Worker);
    return d->channel;
}

void Worker::setChannel(const Icd::ChannelPtr &channel)
{
    Q_D(Worker);
    if (channel != d->channel) {
        d->channel = channel;
        d->workerSend->setChannel(channel);
        d->workerRecv->setChannel(channel);
        emit channelChanged(d->channel);
    }
}

WorkerPtr Worker::relayer() const
{
    Q_D(const Worker);
    return d->workerRecv->relayer();
}

void Worker::setRelayer(const WorkerPtr &relayer)
{
    Q_D(Worker);
    d->workerRecv->setRelayer(relayer);
}

WorkerSendPtr Worker::workerSend() const
{
    Q_D(const Worker);
    return d->workerSend;
}

WorkerRecvPtr Worker::workerRecv() const
{
    Q_D(const Worker);
    return d->workerRecv;
}

bool Worker::isOpen() const
{
    Q_D(const Worker);
    return d->channel->isOpen();
}

bool Worker::open()
{
    Q_D(Worker);
    if (d->channel->isOpen()) {
        return true;
    }

    if (!d->channel->open()) {
        return false;
    }

    emit opened();
    emit toggled(true);

    return true;
}

void Worker::close()
{
    Q_D(Worker);
    if (!d->channel->isOpen()) {
        return;
    }

    d->workerSend->stop();
    d->workerRecv->stop();

    d->channel->close();

    emit closed();
    emit toggled(false);
}

} // end of namespace Icd
