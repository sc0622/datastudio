#include "precomp.h"
#include "JCmdModelSim.h"
#include "JCmdChannelSim.h"
#include "../JCmdChannel.h"
#include "../../channel/JChannel.h"

namespace icdmeta {

// class JCmdModelSimPrivate

class JCmdModelSimPrivate
{
public:
    JCmdModelSimPrivate(JCmdModelSim *q)
        : J_QPTR(q)
        , workMode(JCmdChannel::WorkModeHome)
        , currentWorkMode(JCmdChannel::WorkModeHome)
        , timer(Q_NULLPTR)
    {

    }

    void init();
    static int channelsCount(QQmlListProperty<JCmdChannelSim> *property);
    static JCmdChannelSim *channelsAt(QQmlListProperty<JCmdChannelSim> *property, int index);

    static QSharedPointer<JCmdChannelSim> mapTask(const QSharedPointer<JCmdChannelSim> &channel);

    bool startTime(int msecs);
    void stopTime();

private:
    J_DECLARE_PUBLIC(JCmdModelSim)
    int workMode;
    int currentWorkMode;
    QList<QSharedPointer<JCmdChannelSim> > channels;
    QFutureWatcher<QSharedPointer<JCmdChannelSim> > watcher;
    QTimer *timer;
};

void JCmdModelSimPrivate::init()
{
    Q_Q(JCmdModelSim);
    timer = new QTimer(q);
    timer->setSingleShot(true);
    timer->setTimerType(Qt::PreciseTimer);
    QObject::connect(timer, &QTimer::timeout, q, [=](){
        //
    });
}

int JCmdModelSimPrivate::channelsCount(QQmlListProperty<JCmdChannelSim> *property)
{
    JCmdModelSim *q = qobject_cast<JCmdModelSim *>(property->object);
    return q->J_DPTR->channels.count();
}

JCmdChannelSim *JCmdModelSimPrivate::channelsAt(QQmlListProperty<JCmdChannelSim> *property, int index)
{
    JCmdModelSim *q = qobject_cast<JCmdModelSim *>(property->object);
    return q->J_DPTR->channels[index].data();
}

QSharedPointer<JCmdChannelSim> JCmdModelSimPrivate::mapTask(const QSharedPointer<JCmdChannelSim> &channel)
{
    channel->run();
    return channel;
}

bool JCmdModelSimPrivate::startTime(int msecs)
{
    return QMetaObject::invokeMethod(timer, "start", Qt::QueuedConnection,
                                     Q_ARG(int, msecs));
}

void JCmdModelSimPrivate::stopTime()
{
    QMetaObject::invokeMethod(timer, "stop", Qt::QueuedConnection);
}

// class JCmdModelSim

J_QML_IMPLEMENT_SINGLE_INSTANCE(JCmdModelSim, QQmlEngine::CppOwnership, IcdCore)

JCmdModelSim::JCmdModelSim(QObject *parent)
    : QObject(parent)
    , J_DPTR(new JCmdModelSimPrivate(this))
{
    Q_D(JCmdModelSim);
    d->init();
}

JCmdModelSim::~JCmdModelSim()
{
    Q_D(JCmdModelSim);
    close();
    delete d;
}

void JCmdModelSim::registerQmlType()
{
    //
    IcdMetaRegisterSingletonType2(JCmdModelSim, "JCmdModelSim");

    //
    JCmdChannelSim::registerQmlType();
}

int JCmdModelSim::workMode() const
{
    Q_D(const JCmdModelSim);
    return d->workMode;
}

int JCmdModelSim::currentWorkMode() const
{
    Q_D(const JCmdModelSim);
    return d->currentWorkMode;
}

QQmlListProperty<icdmeta::JCmdChannelSim> JCmdModelSim::channels()
{
    return QQmlListProperty<icdmeta::JCmdChannelSim>(this, 0,
                                                     &JCmdModelSimPrivate::channelsCount,
                                                     &JCmdModelSimPrivate::channelsAt);
}

icdmeta::JCmdChannelSim *JCmdModelSim::findItem(const QString &identity) const
{
    Q_D(const JCmdModelSim);
    for (auto &channel : d->channels) {
        if (channel->identity() == identity) {
            return channel.data();
        }
    }

    return Q_NULLPTR;
}

void JCmdModelSim::setWorkMode(int value)
{
    Q_D(JCmdModelSim);
    if (value != d->workMode) {
        d->workMode = value;
        for (auto &channel : d->channels) {
            channel->setWorkMode(value);
        }
        emit workModeChanged(value);
    }
}

void JCmdModelSim::setCurrentWorkMode(int value)
{
    Q_D(JCmdModelSim);
    if (value != d->currentWorkMode) {
        d->currentWorkMode = value;
        for (auto &channel : d->channels) {
            channel->setCurrentWorkMode(value);
        }
        emit currentWorkModeChanged(value);
    }
}

void JCmdModelSim::appendChannel(icdmeta::JChannel *channel)
{
    Q_D(JCmdModelSim);
    JCmdChannelSim *newChannel = new JCmdChannelSim(channel->identity());
    QQmlEngine::setObjectOwnership(newChannel, QQmlEngine::CppOwnership);
    newChannel->binding(channel);
    d->channels.append(QSharedPointer<JCmdChannelSim>(newChannel, &QObject::deleteLater));
    emit channelsChanged();
}

void JCmdModelSim::clearChannel()
{
    Q_D(JCmdModelSim);
    close();
    if (!d->channels.isEmpty()) {
        d->channels.clear();
        emit channelsChanged();
    }
}

bool JCmdModelSim::open()
{
    Q_D(JCmdModelSim);
    for (auto &channel : d->channels) {
        channel->cancel();
    }

    d->watcher.cancel();
    d->watcher.waitForFinished();
    d->watcher.disconnect(this);

    for (auto &channel : d->channels) {
        if (!channel->open()) {
            return false;
        }
        channel->reset();
    }

    auto future = QtConcurrent::mapped(d->channels, d->mapTask);
    QObject::connect(&d->watcher, &QFutureWatcher<QSharedPointer<JCmdChannelSim> >::finished,
                     this, [=](){
        qDebug() << "command.simulate result:" << d->watcher.result();
    });
    d->watcher.setFuture(future);

    return true;
}

void JCmdModelSim::close()
{
    Q_D(JCmdModelSim);
    for (auto &channel : d->channels) {
        channel->reset();
    }

    d->watcher.cancel();
    d->watcher.waitForFinished();
    d->watcher.disconnect(this);

    for (auto &channel : d->channels) {
        channel->close();
    }
}

void JCmdModelSim::reset()
{
    clearChannel();
}

}
