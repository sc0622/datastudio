#include "precomp.h"
#include "JCmdSysModelSim.h"
#include "JCmdChannelSim.h"
#include "../JCmdChannel.h"
#include "../../channel/JChannel.h"

namespace icdmeta {

// class JCmdSysModelSimPrivate

class JCmdSysModelSimPrivate
{
public:
    JCmdSysModelSimPrivate(JCmdSysModelSim *q)
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
    J_DECLARE_PUBLIC(JCmdSysModelSim)
    int workMode;
    int currentWorkMode;
    QList<QSharedPointer<JCmdChannelSim> > channels;
    QFutureWatcher<QSharedPointer<JCmdChannelSim> > watcher;
    QTimer *timer;
};

void JCmdSysModelSimPrivate::init()
{
    Q_Q(JCmdSysModelSim);
    timer = new QTimer(q);
    timer->setSingleShot(true);
    timer->setTimerType(Qt::PreciseTimer);
    QObject::connect(timer, &QTimer::timeout, q, [=](){
        //
    });
}

int JCmdSysModelSimPrivate::channelsCount(QQmlListProperty<JCmdChannelSim> *property)
{
    JCmdSysModelSim *q = qobject_cast<JCmdSysModelSim *>(property->object);
    return q->J_DPTR->channels.count();
}

JCmdChannelSim *JCmdSysModelSimPrivate::channelsAt(QQmlListProperty<JCmdChannelSim> *property, int index)
{
    JCmdSysModelSim *q = qobject_cast<JCmdSysModelSim *>(property->object);
    return q->J_DPTR->channels[index].data();
}

QSharedPointer<JCmdChannelSim> JCmdSysModelSimPrivate::mapTask(const QSharedPointer<JCmdChannelSim> &channel)
{
    channel->run();
    return channel;
}

bool JCmdSysModelSimPrivate::startTime(int msecs)
{
    return QMetaObject::invokeMethod(timer, "start", Qt::QueuedConnection,
                                     Q_ARG(int, msecs));
}

void JCmdSysModelSimPrivate::stopTime()
{
    QMetaObject::invokeMethod(timer, "stop", Qt::QueuedConnection);
}

// class JCmdSysModelSim

J_QML_IMPLEMENT_SINGLE_INSTANCE(JCmdSysModelSim, QQmlEngine::CppOwnership, IcdCore)

JCmdSysModelSim::JCmdSysModelSim(QObject *parent)
    : QObject(parent)
    , J_DPTR(new JCmdSysModelSimPrivate(this))
{
    Q_D(JCmdSysModelSim);
    d->init();
}

JCmdSysModelSim::~JCmdSysModelSim()
{
    Q_D(JCmdSysModelSim);
    close();
    delete d;
}

void JCmdSysModelSim::registerQmlType()
{
    //
    IcdMetaRegisterSingletonType2(JCmdSysModelSim, "JCmdModelSim");

    //
    JCmdChannelSim::registerQmlType();
}

int JCmdSysModelSim::workMode() const
{
    Q_D(const JCmdSysModelSim);
    return d->workMode;
}

int JCmdSysModelSim::currentWorkMode() const
{
    Q_D(const JCmdSysModelSim);
    return d->currentWorkMode;
}

QQmlListProperty<icdmeta::JCmdChannelSim> JCmdSysModelSim::channels()
{
    return QQmlListProperty<icdmeta::JCmdChannelSim>(this, 0,
                                                     &JCmdSysModelSimPrivate::channelsCount,
                                                     &JCmdSysModelSimPrivate::channelsAt);
}

icdmeta::JCmdChannelSim *JCmdSysModelSim::findItem(const QString &identity) const
{
    Q_D(const JCmdSysModelSim);
    for (auto &channel : d->channels) {
        if (channel->identity() == identity) {
            return channel.data();
        }
    }

    return Q_NULLPTR;
}

void JCmdSysModelSim::setWorkMode(int value)
{
    Q_D(JCmdSysModelSim);
    if (value != d->workMode) {
        d->workMode = value;
        for (auto &channel : d->channels) {
            channel->setWorkMode(value);
        }
        emit workModeChanged(value);
    }
}

void JCmdSysModelSim::setCurrentWorkMode(int value)
{
    Q_D(JCmdSysModelSim);
    if (value != d->currentWorkMode) {
        d->currentWorkMode = value;
        for (auto &channel : d->channels) {
            channel->setCurrentWorkMode(value);
        }
        emit currentWorkModeChanged(value);
    }
}

void JCmdSysModelSim::appendChannel(icdmeta::JChannel *channel)
{
    Q_D(JCmdSysModelSim);
    JCmdChannelSim *newChannel = new JCmdChannelSim(channel->identity());
    QQmlEngine::setObjectOwnership(newChannel, QQmlEngine::CppOwnership);
    newChannel->binding(channel);
    d->channels.append(QSharedPointer<JCmdChannelSim>(newChannel, &QObject::deleteLater));
    emit channelsChanged();
}

void JCmdSysModelSim::clearChannel()
{
    Q_D(JCmdSysModelSim);
    close();
    if (!d->channels.isEmpty()) {
        d->channels.clear();
        emit channelsChanged();
    }
}

bool JCmdSysModelSim::open()
{
    Q_D(JCmdSysModelSim);
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

void JCmdSysModelSim::close()
{
    Q_D(JCmdSysModelSim);
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

void JCmdSysModelSim::reset()
{
    clearChannel();
}

}
