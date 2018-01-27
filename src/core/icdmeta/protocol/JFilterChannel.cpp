#include "precomp.h"
#include "JFilterChannel.h"
#include "../channel/JChannel.h"
#include "JProtocol.h"
#include "../icdmeta_inc.h"

namespace icdmeta {

// class JFilterChannelPrivate

class JFilterChannelPrivate
{
public:
    JFilterChannelPrivate(JFilterChannel *q)
        : q_ptr(q)
        , currentChannel(nullptr)
        , protocol(nullptr)
        , worker(nullptr)
        , timer(nullptr)
        , counter(-1)
    {

    }

    void init();
    static int channelCount(QQmlListProperty<JChannel> *property);
    static JChannel *channelAt(QQmlListProperty<JChannel> *property, int index);

    bool tryOpenChannel(JChannel *channel);

private:
    J_DECLARE_PUBLIC(JFilterChannel)
    QList<JChannel *> channels;
    JChannel *currentChannel;
    JProtocol *protocol;
    Icd::WorkerPtr worker;
    QTimer *timer;
    qint64 counter;
};

void JFilterChannelPrivate::init()
{
    Q_Q(JFilterChannel);
    //
    worker = Icd::WorkerPtr(new Icd::Worker(q));
    worker->workerRecv()->setInterval(20);  // ? ms
    QObject::connect(worker->workerRecv().get(), &Icd::WorkerRecv::started,
                     q, [=](){
        emit q->started();
        emit q->runningChanged();
    });
    QObject::connect(worker->workerRecv().get(), &Icd::WorkerRecv::stopped,
                     q, [=](){
        emit q->stopped();
        emit q->runningChanged();
    });

    //
    timer = new QTimer(q);
    timer->setInterval(2017);       // ? ms (2017, prime number)
    QObject::connect(timer, &QTimer::timeout, q, [=](){
        auto switchChannel = [=]() -> bool {
            bool success = false;
            QListIterator<JChannel *> citer(channels);
            while (citer.hasNext()) {
                JChannel *channel = citer.next();
                if (!currentChannel && channel->isValid()) {
                    success = q->setCurrentChannel(channel);
                } else if (channel == currentChannel) {
                    if (citer.hasNext()) {
                        success = q->setCurrentChannel(citer.next());
                    } else {
                        success = q->setCurrentChannel(channels.first());
                    }
                } else {
                    continue;
                }
                if (success) {
                    if (q->start()) {
                        //
                    } else {
                        //
                    }
                }
                break;
            }

            return success;
        };
        if (!worker->workerRecv()->isRunning()) {
            // error, and switch to a valid channel
            switchChannel();
            return;
        }
        if (!protocol || !protocol->isValid()
                || !currentChannel || !currentChannel->isValid()) {
            return;
        }
        const QSharedPointer<icdmeta::JIcdCounterItem> &counterItem = protocol->table()->counterItem();
        if (!counterItem) {
            return;
        }
        qint64 newCounter = (qint64)counterItem->data();
        //qDebug() << newCounter;
        if (counter != newCounter) {
            counter = newCounter;
            return;
        }
        counter = -1;  // reset counter to original state
        // error, and switch to a valid channel
        switchChannel();
    });
}

int JFilterChannelPrivate::channelCount(QQmlListProperty<JChannel> *property)
{
    JFilterChannel *q = qobject_cast<JFilterChannel *>(property->object);
    return q->d_ptr->channels.count();
}

JChannel *JFilterChannelPrivate::channelAt(QQmlListProperty<JChannel> *property, int index)
{
    JFilterChannel *q = qobject_cast<JFilterChannel *>(property->object);
    return q->d_ptr->channels[index];
}

}

using namespace icdmeta;

// class JFilterChannel

J_QML_IMPLEMENT_SINGLE_INSTANCE(JFilterChannel, QQmlEngine::CppOwnership, IcdCore)

JFilterChannel::JFilterChannel(QObject *parent)
    : QObject(parent)
    , d_ptr(new JFilterChannelPrivate(this))
{
    Q_D(JFilterChannel);
    d->init();
}

JFilterChannel::~JFilterChannel()
{
    Q_D(JFilterChannel);
    delete d;
}

void JFilterChannel::registerQmlType()
{
    jRegisterSingletonTypeCpp2(JFilterChannel);
}

JProtocol *JFilterChannel::protocol() const
{
    Q_D(const JFilterChannel);
    return d->protocol;
}

QQmlListProperty<JChannel> JFilterChannel::channels()
{
    return QQmlListProperty<JChannel>(this, 0,
                                      &JFilterChannelPrivate::channelCount,
                                      &JFilterChannelPrivate::channelAt);
}

JChannel *JFilterChannel::currentChannel() const
{
    Q_D(const JFilterChannel);
    return d->currentChannel;
}

bool JFilterChannel::isRunning() const
{
    Q_D(const JFilterChannel);
    return d->worker->workerRecv()->isRunning();
}

bool JFilterChannel::isActive() const
{
    Q_D(const JFilterChannel);
    return d->timer->isActive();
}

void JFilterChannel::reset()
{
    stop();
    clearChannel();
    setProtocol(nullptr);
}

void JFilterChannel::setProtocol(JProtocol *value)
{
    Q_D(JFilterChannel);
    if (value != d->protocol) {
        if (!value || !value->isValid()) {
            d->worker->workerRecv()->setTable(nullptr);
        } else {
            d->worker->workerRecv()->setTable(value->table()->metaData());
        }
        d->protocol = value;
        emit protocolChanged(value);
    }
}

void JFilterChannel::setCurrentChannel(const QString &mark)
{
    Q_D(JFilterChannel);
    if (!d->timer->isActive()) {
        return;
    }
    for (auto channel : d->channels) {
        if (channel->channel()->mark() == mark) {
            setCurrentChannel(channel);
            break;
        }
    }
}

bool JFilterChannel::setCurrentChannel(JChannel *value)
{
    Q_D(JFilterChannel);
    bool success = false;
    if (value != d->currentChannel) {
        //
        if (d->currentChannel) {
            d->currentChannel->close();
        }
        //
        if (d->protocol) {
            d->protocol->resetBuffer();
        }
        //
        d->currentChannel = value;
        if (!value || !value->isValid()) {
            d->worker->setChannel(nullptr);
        } else {
            if (value->open()) {
                d->worker->setChannel(value->channel()->channel());
                success = true;
            } else {
                success = false;
            }
        }
        emit currentChannelChanged(value);
    }

    return success;
}

void JFilterChannel::appendChannel(JChannel *channel)
{
    Q_D(JFilterChannel);
    if (!d->channels.contains(channel)) {
        d->channels.append(channel);
    }
    emit channelsChanged();
}

void JFilterChannel::removeChannel(const QString &identity)
{
    Q_D(JFilterChannel);
    QMutableListIterator<JChannel *> iter(d->channels);
    while (iter.hasNext()) {
        const JChannel *const channel = iter.next();
        if (channel->identity() == identity) {
            if (channel == d->currentChannel) {
                setCurrentChannel(nullptr);
            }
            iter.remove();
            emit channelsChanged();
            break;
        }
    }
}

void JFilterChannel::clearChannel()
{
    Q_D(JFilterChannel);
    //
    setCurrentChannel(nullptr);
    //
    if (!d->channels.isEmpty()) {
        for (auto channel : d->channels) {
            channel->close();
        }
        d->channels.clear();
        emit channelsChanged();
    }
}

bool JFilterChannel::start()
{
    Q_D(JFilterChannel);
    if (!d->currentChannel) {
        for (auto channel : d->channels) {
            if (channel->isValid()) {
                if (setCurrentChannel(channel)) {
                    break;
                }
            }
        }
        if (!d->currentChannel) {
            return false;
        }
    }

    if (!d->currentChannel->isOpen()) {
        if (!d->currentChannel->open()) {
            return false;
        }
    }

    if (!d->worker->workerRecv()->start()) {
        return false;
    }

    if (!d->timer->isActive()) {
        d->timer->start();
    }

    return true;
}

void JFilterChannel::stop()
{
    Q_D(JFilterChannel);
    if (d->timer->isActive()) {
        d->timer->stop();
        QCoreApplication::removePostedEvents(this, QEvent::Timer);
    }

    d->worker->workerRecv()->stop();
    if (d->currentChannel) {
        d->currentChannel->close();
        setCurrentChannel(nullptr);
    }
}
