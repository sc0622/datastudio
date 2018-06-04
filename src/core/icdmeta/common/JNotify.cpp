#include "precomp.h"
#include "JNotify.h"
#include <QQmlEngine>

namespace icdmeta {

QJSValue jVariantToJSValue(const QVariant &value)
{
    switch (value.type()) {
    case QVariant::Bool:
        return value.toBool();
    case QVariant::Char:
    case QVariant::Int:
        return value.toInt();
    case QVariant::UInt:
        return value.toUInt();
    case QVariant::LongLong:
    case QVariant::ULongLong:
    case QVariant::Double:
        return value.toDouble();
    case QVariant::String:
        return value.toString();
    case QVariant::List:
    default:
        return value.toString();
    }
}

// class JNotifyCallbackData

class JNotifyCallbackData
{
public:
    JNotifyCallbackData()
    {

    }

    explicit JNotifyCallbackData(JNotifyCallback callback)
        : callback(callback)
    {

    }

    explicit JNotifyCallbackData(QJSValue callback)
        : jsCallback(callback)
    {

    }

    JNotifyCallbackData(const JNotifyCallbackData &other)
    {
        *this = other;
    }

    JNotifyCallbackData &operator =(const JNotifyCallbackData &other)
    {
        if (this == &other) {
            return *this;
        }
        callback = other.callback;
        jsCallback = other.jsCallback;
        return *this;
    }

    void call(JNEvent &event)
    {
        if (callback) {
            callback(event);
        } else if (jsCallback.isCallable()) {
            jsCallback.call(event.toList());    //TODO
        }
    }

    void operator ()(JNEvent &event)
    {
        call(event);
    }

    bool isValid() const
    {
        return (callback || jsCallback.isCallable());
    }

private:
    JNotifyCallback callback;
    QJSValue jsCallback;
};

// class JNEvent

class JNEvent;
typedef std::shared_ptr<JNEvent> JNEventPtr;

JNEvent::JNEvent()
{

}

JNEvent::JNEvent(const QString &channel, const QVariant &data)
    : _channel(channel)
    , _argument(data)
{

}

JNEvent::~JNEvent()
{

}

QJSValueList JNEvent::toList() const
{
    QJSValueList args;
    args.append(_channel);
    args.append(jVariantToJSValue(_argument));
    args.append(jVariantToJSValue(_returnValue));
    return args;
}

// class JPostMsgEvent

class JPostMsgEvent : public QEvent
{
public:
    JPostMsgEvent(JNotifyCallback callback, const QString &channel,
                  const QVariant &argument)
        : QEvent(static_cast<QEvent::Type>(JNotify::Evt_PostMsg))
        , callback(callback)
        , channel(channel)
        , argument(argument)
    {

    }

    JPostMsgEvent(QJSValue callback, const QString &channel,
                  const QVariant &argument)
        : QEvent(static_cast<QEvent::Type>(JNotify::Evt_PostMsg))
        , callback(callback)
        , channel(channel)
        , argument(argument)
    {

    }

    void execute()
    {
        callback(JNEvent(channel, argument));
    }

private:
    JNotifyCallbackData callback;
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
    JNotifyCallbackData callback;
    bool sync;

    explicit JNotifyData()
        : sync(true)
    {

    }

    JNotifyData(JNotifyCallback callback, bool sync)
        : callback(callback)
        , sync(sync)
    {

    }

    JNotifyData(QJSValue callback, bool sync)
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

    bool isValid() const
    {
        return callback.isValid();
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
        if (notifyData->isValid()) {
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
        if (notifyData->isValid()) {
            QCoreApplication::postEvent(q, new JPostMsgEvent(notifyData->callback, channel, data));
        }
    }
}

// class JNotify

void JNotify::registerQmlType()
{
    IcdMetaRegisterUncreatableType2(JNotify);
}

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

void JNotify::postJs(const QString &channel, const QString &argument)
{
    post(channel, nullptr, QVariant(argument));
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

void JNotify::clear()
{
    QCoreApplication::removePostedEvents(this, Evt_PostMsg);
    Q_D(JNotify);
    d->mutex.lock();
    d->callbacks.clear();
    d->pendings.clear();
    d->mutex.unlock();
}

void JNotify::on(const QString &channel, QJSValue callback, bool sync)
{
    on(channel, nullptr, callback, sync);
}

void JNotify::on(const QString &channel, QObject *receiver, QJSValue callback, bool sync)
{
    Q_D(JNotify);
    // auto-removing
    if (receiver) {
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
            const QJSValueList arguments = event->toList();
            d->mutex.unlock();
            if (sync) {
                callback.call(arguments);
            } else {
                QCoreApplication::postEvent(this, new JPostMsgEvent(callback, channel, argument));
            }
            d->mutex.lock();
            iterPendings.remove();
        }
    }
    d->mutex.unlock();
}

QJSValue JNotify::jsSend(const QString &channel, const QJSValue &argument)
{
    return jVariantToJSValue(send(channel, nullptr, argument.toVariant()));
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
