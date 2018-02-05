#include "precomp.h"
#include "JDataChannel.h"
#include "icdworker/icdworker.h"
#include "../core/jicdtable.h"
#include "../channel/JChannel.h"
#include "../channel/JSuperChannel.h"
#include "JProtocol.h"

namespace icdmeta {

// class JDataChannelPrivate

class JDataChannelPrivate
{
public:
    JDataChannelPrivate(JDataChannel *q)
        : q_ptr(q)
        , channel(nullptr)
        , protocolSend(nullptr)
        , protocolRecv(nullptr)
        , worker(nullptr)
    {

    }

    void init();
    void setChannel(JChannel *channel);
    void setProtocolSend(JProtocol *protocol);
    void setProtocolRecv(JProtocol *protocol);
    void setWorker(const Icd::WorkerPtr &worker);

private:
    J_DECLARE_PUBLIC(JDataChannel)
    QMutex mutex;
    QString identity;
    JChannel *channel;
    JProtocol *protocolSend;
    JProtocol *protocolRecv;
    Icd::WorkerPtr worker;
};

void JDataChannelPrivate::init()
{

}

void JDataChannelPrivate::setChannel(JChannel *channel)
{
    Q_Q(JDataChannel);
    if (channel != this->channel) {
        if (this->channel) {
            this->channel->close();
        }
        this->channel = channel;
        emit q->channelChanged();
    }
}

void JDataChannelPrivate::setProtocolSend(JProtocol *protocol)
{
    Q_Q(JDataChannel);
    if (protocol != this->protocolSend) {
        this->protocolSend = protocol;
        emit q->protocolSendChanged();
    }
}

void JDataChannelPrivate::setProtocolRecv(JProtocol *protocol)
{
    Q_Q(JDataChannel);
    if (protocol != this->protocolRecv) {
        this->protocolRecv = protocol;
        emit q->protocolRecvChanged();
    }
}

void JDataChannelPrivate::setWorker(const Icd::WorkerPtr &worker)
{
    if (worker != this->worker) {
        if (this->worker) {
            this->worker->close();
            Q_Q(JDataChannel);
            this->worker->disconnect(q);
        }
        this->worker = worker;
    }
}

}

using namespace icdmeta;

// class JDataChannel

JDataChannel::JDataChannel(const QString &identity, QObject *parent)
    : QObject(parent)
    , d_ptr(new JDataChannelPrivate(this))
{
    Q_D(JDataChannel);
    d->identity = identity;
    d->init();
}

JDataChannel::~JDataChannel()
{
    Q_D(JDataChannel);
    delete d;
}

void JDataChannel::registerQmlType()
{
    jRegisterUncreatableType(JDataChannel);
}

QString JDataChannel::identity() const
{
    Q_D(const JDataChannel);
    return d->identity;
}

JChannel *JDataChannel::channel() const
{
    Q_D(const JDataChannel);
    return d->channel;
}

JProtocol *JDataChannel::protocolSend() const
{
    Q_D(const JDataChannel);
    return d->protocolSend;
}

JProtocol *JDataChannel::protocolRecv() const
{
    Q_D(const JDataChannel);
    return d->protocolRecv;
}

bool JDataChannel::binding(JChannel *channel, JProtocol *protocolSend, JProtocol *protocolRecv)
{
    if (!channel || !protocolSend || !protocolRecv) {
        return false;
    }

    if (!channel->channel()) {
        Q_ASSERT(false);
        return false;
    }

    if (!protocolSend->isValid() || !protocolRecv->isValid()) {
        return false;
    }

    icdmeta::JIcdTable *tableSend = protocolSend->table();
    icdmeta::JIcdTable *tableRecv = protocolRecv->table();
    if (!tableSend || !tableRecv) {
        return false;
    }

    Q_D(JDataChannel);
    if (d->worker) {
        return false;
    }

    d->setChannel(channel);
    d->setProtocolSend(protocolSend);
    d->setProtocolRecv(protocolRecv);

    // worker
    if (!d->worker) {
        Icd::WorkerPtr worker(new Icd::Worker(channel->channel()->channel(), this));
        connect(worker.get(), &Icd::Worker::toggled,
                channel->channel(), &JSuperChannel::isOpenChanged);
        d->setWorker(worker);
    }

    d->worker->workerSend()->setTable(tableSend->metaData());
    d->worker->workerRecv()->setTable(tableRecv->metaData());

    return true;
}

void JDataChannel::unbinding()
{
    Q_D(JDataChannel);
    d->setWorker(nullptr);
    d->setChannel(nullptr);
    d->setProtocolSend(nullptr);
    d->setProtocolRecv(nullptr);
}

void JDataChannel::unbindingSend()
{
    Q_D(JDataChannel);
    d->setProtocolSend(nullptr);
}

void JDataChannel::unbindingRecv()
{
    Q_D(JDataChannel);
    d->setProtocolRecv(nullptr);
}

void JDataChannel::setCounterLoop(bool loop)
{
    Q_D(JDataChannel);
    if (!d->worker) {
        return;
    }

    d->worker->workerSend()->setCounterLoop(loop);
}

void JDataChannel::setFrameLoop(bool loop)
{
    Q_D(JDataChannel);
    if (!d->worker) {
        return;
    }

    d->worker->workerSend()->setFrameLoop(loop);
}

bool JDataChannel::open()
{
    Q_D(JDataChannel);
    if (!d->worker) {
        return false;
    }
    return d->worker->open();
}

void JDataChannel::close()
{
    Q_D(JDataChannel);
    if (!d->worker) {
        return;
    }
    d->worker->close();
}

bool JDataChannel::start(int intervalSend, int intervalRecv)
{
    if (!startSend(intervalSend)) {
        return false;
    }

    return startRecv(intervalRecv);
}

bool JDataChannel::startSend(int interval)
{
    Q_D(JDataChannel);
    if (!d->worker || !d->protocolSend || !d->protocolSend->isValid()) {
        return false;
    }

    if (!d->worker->open()) {
        return false;
    }

    Icd::WorkerSendPtr workerSend = d->worker->workerSend();
    icdmeta::JIcdTable *table = d->protocolSend->table();
    if (table) {
        workerSend->setTable(table->metaData());
    }

    if (interval > 0) {
        workerSend->setInterval(interval);
    }

    return workerSend->start();
}

bool JDataChannel::startRecv(int interval)
{
    Q_D(JDataChannel);
    if (!d->worker || !d->protocolRecv || !d->protocolRecv->isValid()) {
        return false;
    }

    if (!d->worker->open()) {
        return false;
    }

    Icd::WorkerRecvPtr workerRecv = d->worker->workerRecv();
    icdmeta::JIcdTable *table = d->protocolRecv->table();
    if (table) {
        workerRecv->setTable(table->metaData());
    }

    if (interval > 0) {
        workerRecv->setInterval(interval);
    }

    return workerRecv->start();
}

void JDataChannel::stop()
{
    stopSend();
    stopRecv();
}

void JDataChannel::stopSend()
{
    Q_D(JDataChannel);
    if (!d->worker) {
        return;
    }

    return d->worker->workerSend()->stop();
}

void JDataChannel::stopRecv()
{
    Q_D(JDataChannel);
    if (!d->worker) {
        return;
    }

    return d->worker->workerRecv()->stop();
}

bool JDataChannel::sendOnce(bool counterLoop, bool frameLoop)
{
    Q_D(JDataChannel);
    if (!d->channel || !d->protocolSend) {
        return false;
    }

    if (!d->channel->isOpen() || !d->protocolSend->isValid()) {
        return false;
    }

    return d->worker->workerSend()->sendOnce(counterLoop, frameLoop);
}

bool JDataChannel::sendOnce(const QByteArray &data)
{
    if (data.isEmpty()) {
        return true;
    }

    Q_D(JDataChannel);
    if (!d->channel || !d->channel->isOpen()) {
        return false;
    }

    return d->channel->writeData(data);
}
