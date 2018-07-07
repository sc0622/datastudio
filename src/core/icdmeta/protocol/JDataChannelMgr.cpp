#include "precomp.h"
#include "JDataChannelMgr.h"
#include "JDataChannel.h"
#include "../channel/JChannel.h"
#include "JProtocol.h"
#include "../core/jicdtable.h"
#include <QtConcurrent>
#include "../common/JSearchWatcher.h"
#include "../common/private/jsearchwatcher_p.h"

namespace icdmeta {

// class JDataChannelMgrPrivate

class JDataChannelMgrPrivate
{
public:
    JDataChannelMgrPrivate(JDataChannelMgr *q)
        : J_QPTR(q)
    {

    }

    void init();
    static int channelCount(QQmlListProperty<JDataChannel> *property);
    static JDataChannel *channelAt(QQmlListProperty<JDataChannel> *property, int index);
    bool searchItem(icdmeta::JIcdTable *table, QObject *target, QJSValue callback);

private:
    J_DECLARE_PUBLIC(JDataChannelMgr)
    JDataChannelPtrArray channels;
    QList<QSharedPointer<JSearchWatcher> > watchers;
};

void JDataChannelMgrPrivate::init()
{

}

int JDataChannelMgrPrivate::channelCount(QQmlListProperty<JDataChannel> *property)
{
    JDataChannelMgr *q = qobject_cast<JDataChannelMgr *>(property->object);
    return q->J_DPTR->channels.count();
}

JDataChannel *JDataChannelMgrPrivate::channelAt(QQmlListProperty<JDataChannel> *property, int index)
{
    JDataChannelMgr *q = qobject_cast<JDataChannelMgr *>(property->object);
    return q->J_DPTR->channels[index].data();
}

bool JDataChannelMgrPrivate::searchItem(icdmeta::JIcdTable *table, QObject *target,
                                        QJSValue callback)
{
    if (!table || !target) {
        return false;
    }

    Q_Q(JDataChannelMgr);

    QStringList properties;
    const QMetaObject *metaItems = target->metaObject();
    for (int i = metaItems->propertyOffset(); i < metaItems->propertyCount(); ++i) {
        const QString name = QString::fromLatin1(metaItems->property(i).name());
        if (name.startsWith('_') || name == "objectName") {
            continue;
        }
        properties << name;
    }

    if (properties.isEmpty()) {
        return true;
    }

    QSharedPointer<JSearchWatcher> newWatcher = QSharedPointer<JSearchWatcher>
            (new JSearchWatcher(callback, target, nullptr), jdelete_qobject);
    QQmlEngine::setObjectOwnership(newWatcher.data(), QQmlEngine::CppOwnership);
    watchers.push_back(newWatcher);

    QFuture<QString> future = QtConcurrent::filtered(
                properties, [=](const QString &property) -> bool {
        icdmeta::JIcdObject *childItem = table->itemByMark(property, true);
        if (!childItem) {
            return true;
        }
        if (!newWatcher->isValid()) {
            return false;
        }
        QCoreApplication::postEvent(newWatcher.data(), new SetPropertyEvent(
                                        property, QVariant::fromValue(childItem), newWatcher));
        return false;
    });
    auto reduce = [=](){
        const QStringList result = newWatcher->future().results();
        QJSValue _callback = newWatcher->callback();
        if (!_callback.isCallable()) {
            return;
        }
        //
        QJSValueList args;
        // success
        args.append(result.isEmpty());
        // message
        if (!result.isEmpty()) {
            args.append(result.join("<br/>"));
        }
        QCoreApplication::postEvent(q, new JCallbackEvent(_callback, args));
    };
    auto deleteWatcher = [=](bool canceled){
        if (canceled) {
            QCoreApplication::removePostedEvents(newWatcher.data(), Event_SetProperty);
        }
        newWatcher->disconnect(q);
        watchers.removeOne(newWatcher);
    };
    QObject::connect(newWatcher.data(), &QFutureWatcher<QString>::finished, q, [=](){
        reduce();
        deleteWatcher(false);
    });
    QObject::connect(newWatcher.data(), &QFutureWatcher<QString>::canceled, q, [=](){
        deleteWatcher(true);
    });
    newWatcher->setFuture(future);

    return true;
}

}

using namespace icdmeta;

// class JDataChannelMgr

J_QML_IMPLEMENT_SINGLE_INSTANCE(JDataChannelMgr, QQmlEngine::CppOwnership, IcdCore)

JDataChannelMgr::JDataChannelMgr(QObject *parent)
    : QObject(parent)
    , J_DPTR(new JDataChannelMgrPrivate(this))
{
    Q_D(JDataChannelMgr);
    d->init();
}

JDataChannelMgr::~JDataChannelMgr()
{
    Q_D(JDataChannelMgr);
    cancelWatcher();
    delete d;
}

void JDataChannelMgr::registerQmlType()
{
    IcdMetaRegisterSingletonType3(JDataChannelMgr);

    JDataChannel::registerQmlType();
}

QQmlListProperty<JDataChannel> JDataChannelMgr::channels()
{
    return QQmlListProperty<JDataChannel>(this, nullptr,
                                          &JDataChannelMgrPrivate::channelCount,
                                          &JDataChannelMgrPrivate::channelAt);
}

JDataChannel *JDataChannelMgr::binding(JChannel *channel, JProtocol *protocolSend,
                                       JProtocol *protocolRecv)
{
    if (!channel || !protocolSend || !protocolRecv) {
        return nullptr;
    }

    Q_D(JDataChannelMgr);
    QSharedPointer<JDataChannel> newChannel =
            QSharedPointer<JDataChannel>(new JDataChannel(channel->identity()),
                                         jdelete_qobject);
    QQmlEngine::setObjectOwnership(newChannel.data(), QQmlEngine::CppOwnership);
    bool result = newChannel->binding(channel, protocolSend, protocolRecv);
    if (!result) {
        return nullptr;
    }

    d->channels.append(newChannel);

    emit channelsChanged();
    emit channelBound(newChannel->identity());

    return newChannel.data();
}

JDataChannel *JDataChannelMgr::bindingRecv(JChannel *channel, JProtocol *protocol)
{
    if (!channel || !protocol) {
        return nullptr;
    }

    Q_D(JDataChannelMgr);
    QSharedPointer<JDataChannel> newChannel =
            QSharedPointer<JDataChannel>(new JDataChannel(channel->identity()),
                                         jdelete_qobject);
    QQmlEngine::setObjectOwnership(newChannel.data(), QQmlEngine::CppOwnership);
    bool result = newChannel->bindingRecv(channel, protocol);
    if (!result) {
        return nullptr;
    }

    d->channels.append(newChannel);

    emit channelsChanged();
    emit channelBound(newChannel->identity());

    return newChannel.data();
}

void JDataChannelMgr::unbinding(const QString &channelId)
{
    Q_D(JDataChannelMgr);
    QMutableListIterator<QSharedPointer<JDataChannel> > citer(d->channels);
    while (citer.hasNext()) {
        const QSharedPointer<JDataChannel> &dataChannel = citer.next();
        if (!dataChannel) {
            continue;
        }
        const QString dataChannelId = dataChannel->identity();
        const JChannel *channel = dataChannel->channel();
        if (!channel) {
            continue;
        }
        if (channel->identity() == channelId) {
            dataChannel->unbinding();
            citer.remove();
            emit channelsChanged();
            emit channelUnbound(dataChannelId);
            break;
        }
    }
}

JDataChannel *JDataChannelMgr::channelIdOf(const QString &channelId) const
{
    Q_D(const JDataChannelMgr);
    for (auto &channel : d->channels) {
        if (!channel) {
            continue;
        }
        if (channel->identity() == channelId) {
            return channel.data();
        }
    }

    return nullptr;
}

void JDataChannelMgr::cancelWatcher()
{
    Q_D(JDataChannelMgr);
    while (!d->watchers.isEmpty()) {
        auto watcher = d->watchers.takeFirst();
        watcher->cancel();
        QCoreApplication::removePostedEvents(watcher.data(), Event_SetProperty);
        watcher->disconnect(this);
        watcher->waitForFinished();
        d->watchers.removeOne(watcher);
    }
}

bool JDataChannelMgr::search(icdmeta::JIcdTable *table, QObject *target, QJSValue callback)
{
    Q_D(JDataChannelMgr);
    return d->searchItem(table, target, callback);
}

void JDataChannelMgr::cancelSearch(QObject *target)
{
    Q_D(JDataChannelMgr);
    for (auto watcher : d->watchers) {
        if (watcher->target() == target) {
            watcher->cancel();
            QCoreApplication::removePostedEvents(watcher.data(), Event_SetProperty);
            watcher->disconnect(this);
            watcher->waitForFinished();
            d->watchers.removeOne(watcher);
        }
    }
}

void JDataChannelMgr::cancelSearch(const QList<QObject *> targets)
{
    for (auto target : targets) {
        cancelSearch(target);
    }
}

void JDataChannelMgr::resetTarget(QObject *target)
{
    if (!target) {
        return;
    }

    const QMetaObject *metaItems = target->metaObject();
    for (int i = metaItems->propertyOffset(); i < metaItems->propertyCount(); ++i) {
        const char *_name = metaItems->property(i).name();
        const QString name = QString::fromLatin1(_name);
        if (name.startsWith('_') || name == "objectName") {
            continue;   // ignore internal property
        }
        target->setProperty(_name, QVariant::Invalid);
    }
}

void JDataChannelMgr::clearBinding()
{
    Q_D(JDataChannelMgr);
    if (!d->channels.isEmpty()) {
        QMutableListIterator<QSharedPointer<JDataChannel> > citer(d->channels);
        while (citer.hasNext()) {
            const QSharedPointer<JDataChannel> &channel = citer.next();
            if (!channel) {
                continue;
            }
            channel->unbinding();
            channel->close();
            emit channelUnbound(channel->identity());
        }
        d->channels.clear();
        emit channelsChanged();
    }
}

void JDataChannelMgr::reset()
{
    cancelWatcher();
    clearBinding();
}

void JDataChannelMgr::customEvent(QEvent *event)
{
    switch (static_cast<int>(event->type())) {
    case Event_Callback:
    {
        JCallbackEvent *cbEvent = reinterpret_cast<JCallbackEvent *>(event);
        if (!cbEvent) {
            break;
        }
        cbEvent->accept();
        cbEvent->execute();
        break;
    }
    default:
        break;
    }
}
