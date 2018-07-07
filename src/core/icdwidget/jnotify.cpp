#include "precomp.h"
#include "jnotify.h"

namespace Icd {

// class JNEvent

class JNEvent;
typedef std::shared_ptr<JNEvent> JNEventPtr;

JNEvent::JNEvent()
    : _returnValue(false)
{

}

JNEvent::JNEvent(const QString &channel, const QVariant &data)
    : _channel(channel)
    , _argument(data)
    , _returnValue(false)
{

}

JNEvent::~JNEvent()
{
    if (_disposeCallback) {
        _disposeCallback();
    }
}

// class JPostMsgEvent

class JPostMsgEvent : public QEvent
{
public:
    explicit JPostMsgEvent(JNotifyCallback callback, const QString &channel,
                           const QVariant &argument)
        : QEvent(static_cast<QEvent::Type>(JNotify::Evt_PostMsg))
        , callback(callback)
        , channel(channel)
        , argument(argument)
    {

    }

    void execute()
    {
        if (callback) {
            callback(JNEvent(channel, argument));
        }
    }

private:
    JNotifyCallback callback;
    QString channel;
    QVariant argument;
};

// class JNotifyData

class JNotifyData;
typedef std::shared_ptr<JNotifyData> JNotifyDataPtr;
typedef std::list<JNotifyDataPtr> JNotifyDataPtrArray;

class JNotifyData
{
public:
    JNotifyCallback callback;
    bool sync;

    explicit JNotifyData()
        : callback(nullptr)
        , sync(true)
    {

    }

    JNotifyData(JNotifyCallback callback, bool sync)
        : callback(callback)
        , sync(sync)
    {

    }

    JNotifyData(const JNotifyData &other)
    {
        *this = other;
    }

    JNotifyData &operator =(const JNotifyData &other)
    {
        if (this == &other) {
            return *this;
        }
        callback = other.callback;
        sync = other.sync;
        return *this;
    }
};

// class JNotifyPrivate

class JNotifyPrivate
{
public:
    JNotifyPrivate(JNotify *q)
        : J_QPTR(q)
        , mutex(QMutex::Recursive)
    {

    }

    void init();
    QVariant send(const JNotifyDataPtrArray &notifyDatas,
                  const QString &channel, const QVariant &argument);
    void post(const JNotifyDataPtrArray &notifyDatas,
              const QString &channel, const QVariant &argument);

private:
    J_DECLARE_PUBLIC(JNotify)
    QMutex mutex;
    QMap<QString, QMap<QObject* /*receiver*/,JNotifyDataPtrArray> > callbacks;
    QQueue<JNEventPtr> pendings;
    static QMap<QString, JNotify *> insts;
};

QMap<QString, JNotify *> JNotifyPrivate::insts;

void JNotifyPrivate::init()
{

}

QVariant JNotifyPrivate::send(const JNotifyDataPtrArray &notifyDatas, const QString &channel,
                              const QVariant &argument)
{
    Q_Q(JNotify);
    QVariant ret;
    foreach (auto notifyData, notifyDatas) {
        if (notifyData->callback) {
            if (notifyData->sync) {
                JNEvent event(channel, argument);
                notifyData->callback(event);
                return event.returnValue();
            } else {
                QCoreApplication::postEvent(q, new JPostMsgEvent(notifyData->callback, channel, argument));
            }
        }
    }
    return ret;
}

void JNotifyPrivate::post(const JNotifyDataPtrArray &notifyDatas, const QString &channel, const QVariant &data)
{
    Q_Q(JNotify);
    foreach (auto notifyData, notifyDatas) {
        if (notifyData->callback) {
            QCoreApplication::postEvent(q, new JPostMsgEvent(notifyData->callback, channel, data));
        }
    }
}

// class JNotify

JNotify *JNotify::inst(const QString &name, QObject *parent)
{
    if (name.isEmpty()) {
        return nullptr;
    }

    QMap<QString, JNotify *>::ConstIterator citer = JNotifyPrivate::insts.find(name);
    if (citer != JNotifyPrivate::insts.constEnd()) {
        return citer.value();
    } else {
        JNotify *newInst = new JNotify(parent);
        JNotifyPrivate::insts[name] = newInst;
        return newInst;
    }
}

void JNotify::on(const QString &channel, JNotifyCallback callback, bool sync)
{
    on(channel, nullptr, callback, sync);
}

void JNotify::on(const QString &channel, QObject *receiver, JNotifyCallback callback, bool sync)
{
    Q_D(JNotify);
    // auto-removing
    if (receiver) {
        receiver->disconnect(this);
        connect(receiver, &QObject::destroyed, this, [=](QObject *object){
            un(object);
        });
    }
    d->mutex.lock();
    // callbacks
    const JNotifyDataPtr notifyData = JNotifyDataPtr(new JNotifyData(callback, sync));
    auto iterCalbacks = d->callbacks.find(channel);
    if (iterCalbacks != d->callbacks.end()) {
        QMap<QObject*,JNotifyDataPtrArray> &receivers = iterCalbacks.value();
        auto iterDatas = receivers.find(receiver);
        if (iterDatas != receivers.end()) {
            JNotifyDataPtrArray &datas = iterDatas.value();
            datas.push_back(notifyData);
        } else {
            JNotifyDataPtrArray datas;
            datas.push_back(notifyData);
            receivers[receiver] = datas;
        }
    } else {
        JNotifyDataPtrArray datas;
        datas.push_back(notifyData);
        QMap<QObject*,JNotifyDataPtrArray> receivers;
        receivers[receiver] = datas;
        d->callbacks[channel] = receivers;
    }
    // pendings
    QMutableListIterator<JNEventPtr> iterPendings(d->pendings);
    while (iterPendings.hasNext()) {
        const JNEventPtr &event = iterPendings.next();
        if (channel == event->channel()) {
            const QVariant argument = event->argument();
            d->mutex.unlock();
            if (sync) {
                callback(JNEvent(channel, argument));
            } else {
                QCoreApplication::postEvent(this, new JPostMsgEvent(callback, channel, argument));
            }
            d->mutex.lock();
            iterPendings.remove();
        }
    }
    d->mutex.unlock();
}

void JNotify::once(const QString &channel, JNotifyCallback callback, bool sync)
{
    once(channel, nullptr, callback, sync);
}

void JNotify::once(const QString &channel, QObject *receiver, JNotifyCallback callback, bool sync)
{
    Q_UNUSED(channel);
    Q_UNUSED(receiver);
    Q_UNUSED(callback);
    Q_UNUSED(sync);
    Q_ASSERT_X(false, "JNotify", "Not supported!");
}

QVariant JNotify::send(const QString &channel, const QVariant &data)
{
    return send(channel, nullptr, data);
}

QVariant JNotify::send(const QString &channel, QObject *receiver, const QVariant &argument)
{
    Q_D(JNotify);
    QMutexLocker locker(&d->mutex);
    auto citerCallbacks = d->callbacks.find(channel);
    if (citerCallbacks != d->callbacks.cend()) {
        QVariant ret;
        const QMap<QObject*,JNotifyDataPtrArray> &receivers = citerCallbacks.value();
        if (receiver) {
            auto citerReceivers = receivers.find(receiver);
            if (citerReceivers != receivers.end()) {
                ret = d->send(citerReceivers.value(), channel, argument);
            }
        } else {
            QMapIterator<QObject*,JNotifyDataPtrArray> citerReceivers(receivers);
            while (citerReceivers.hasNext()) {
                citerReceivers.next();
                ret = d->send(citerReceivers.value(), channel, argument);
            }
        }
        return ret;
    } else {
        d->pendings.append(JNEventPtr(new JNEvent(channel, argument)));
        return QVariant();
    }

    return QVariant();
}

void JNotify::post(const QString &channel, const QVariant &data)
{
    post(channel, nullptr, data);
}

void JNotify::post(const QString &channel, QObject *receiver, const QVariant &argument)
{
    Q_D(JNotify);
    QMutexLocker locker(&d->mutex);
    auto citerCallbacks = d->callbacks.find(channel);
    if (citerCallbacks != d->callbacks.cend()) {
        const QMap<QObject*,JNotifyDataPtrArray> &receivers = citerCallbacks.value();
        if (receiver) {
            auto citerReceivers = receivers.find(receiver);
            if (citerReceivers != receivers.end()) {
                d->post(citerReceivers.value(), channel, argument);
            }
        } else {
            QMapIterator<QObject*,JNotifyDataPtrArray> citerReceivers(receivers);
            while (citerReceivers.hasNext()) {
                citerReceivers.next();
                d->post(citerReceivers.value(), channel, argument);
            }
        }
    } else {
        d->pendings.append(JNEventPtr(new JNEvent(channel, argument)));
    }
}

bool JNotify::has(const QString &channel) const
{
    Q_D(const JNotify);
    QMutexLocker locker(&const_cast<QMutex&>(d->mutex));
    return d->callbacks.contains(channel);
}

void JNotify::un(const QString &channel)
{
    Q_D(JNotify);
    d->mutex.lock();
    d->callbacks.remove(channel);
    d->mutex.unlock();
}

void JNotify::un(QObject *receiver)
{
    if (!receiver) {
        return;
    }
    Q_D(JNotify);
    receiver->disconnect(this);
    d->mutex.lock();
    QMutableMapIterator<QString, QMap<QObject*,JNotifyDataPtrArray> > iterCallbacks(d->callbacks);
    while (iterCallbacks.hasNext()) {
        iterCallbacks.next();
        QMap<QObject*,JNotifyDataPtrArray> &receivers = iterCallbacks.value();
        QMutableMapIterator<QObject*,JNotifyDataPtrArray> iterReceivers(receivers);
        while (iterReceivers.hasNext()) {
            iterReceivers.next();
            if (iterReceivers.key() == receiver) {
                iterReceivers.remove();
                break;
            }
        }
        if (receivers.isEmpty()) {
            iterCallbacks.remove();
        }
    }
    d->mutex.unlock();
}

void JNotify::un(QObject *receiver, const QString &channel)
{
    if (!receiver) {
        return;
    }
    Q_D(JNotify);
    d->mutex.lock();
    auto iterCallback = d->callbacks.find(channel);
    if (iterCallback != d->callbacks.end()) {
        QMap<QObject*,JNotifyDataPtrArray> &receivers = iterCallback.value();
        QMutableMapIterator<QObject*,JNotifyDataPtrArray> iterReceivers(receivers);
        while (iterReceivers.hasNext()) {
            iterReceivers.next();
            if (iterReceivers.key() == receiver) {
                receiver->disconnect(this);
                iterReceivers.remove();
                break;
            }
        }
        if (receivers.isEmpty()) {
            d->callbacks.erase(iterCallback);
        }
    }
    d->mutex.unlock();
}

void JNotify::clear()
{
    QCoreApplication::removePostedEvents(this, Evt_PostMsg);
    Q_D(JNotify);
    d->mutex.lock();
    d->callbacks.clear();
    d->pendings.clear();
    d->mutex.unlock();
}

void JNotify::customEvent(QEvent *event)
{
    switch (event->type()) {
    case Evt_PostMsg:
    {
        JPostMsgEvent *pmEvent = reinterpret_cast<JPostMsgEvent *>(event);
        if (!pmEvent) {
            break;
        }
        pmEvent->execute();
        break;
    }
    default:
        break;
    }
}

JNotify::JNotify(QObject *parent)
    : QObject(parent)
    , J_DPTR(new JNotifyPrivate(this))
{
    Q_D(JNotify);
    d->init();
}

JNotify::~JNotify()
{
    Q_D(JNotify);
    delete d;
}

}
