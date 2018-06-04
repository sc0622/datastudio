#include "precomp.h"
#include "JChannelPool.h"
#include "JChannel.h"
#include "JSuperChannel.h"

namespace icdmeta {

// class JChannelPoolPrivate

class JChannelPoolPrivate
{
public:
    JChannelPoolPrivate(JChannelPool *q)
        : J_QPTR(q)
    {

    }

    void init();
    void clearChannel();
    static int channelCount(QQmlListProperty<JChannel> *property);
    static JChannel *channelAt(QQmlListProperty<JChannel> *property, int index);

private:
    J_DECLARE_PUBLIC(JChannelPool)
    JChannelPtrArray channels;
};

void JChannelPoolPrivate::init()
{

}

void JChannelPoolPrivate::clearChannel()
{
    foreach (auto &channel, channels) {
        if (channel) {
            channel->close();
        }
    }
    channels.clear();
    Q_Q(JChannelPool);
    emit q->channelsChanged();
}

int JChannelPoolPrivate::channelCount(QQmlListProperty<JChannel> *property)
{
    JChannelPool *q = qobject_cast<JChannelPool *>(property->object);
    return q->J_DPTR->channels.count();
}

JChannel *JChannelPoolPrivate::channelAt(QQmlListProperty<JChannel> *property, int index)
{
    JChannelPool *q = qobject_cast<JChannelPool *>(property->object);
    return q->J_DPTR->channels[index].data();
}

}

using namespace icdmeta;

// class JChannelPool

J_QML_IMPLEMENT_SINGLE_INSTANCE(JChannelPool, QQmlEngine::CppOwnership, IcdCore)

JChannelPool::JChannelPool(QObject *parent)
    : QObject(parent)
    , J_DPTR(new JChannelPoolPrivate(this))
{
    Q_D(JChannelPool);
    d->init();
}

JChannelPool::~JChannelPool()
{
    Q_D(JChannelPool);
    delete d;
}

void JChannelPool::registerQmlType()
{
    IcdMetaRegisterSingletonType3(JChannelPool);

    JChannel::registerQmlType();
}

QQmlListProperty<JChannel> JChannelPool::channels()
{
    return QQmlListProperty<JChannel>(this, nullptr,
                                      &JChannelPoolPrivate::channelCount,
                                      &JChannelPoolPrivate::channelAt);
}

bool JChannelPool::loadConfig(const QString &filePath, const QString &nodePath)
{
    Q_D(JChannelPool);

    d->channels.clear();
    emit channelsChanged();

    const Json::Value channelJson = Json::resolve(filePath.toStdString(), nodePath.toStdString());
    if (channelJson == Json::Value::null || !channelJson.isArray()) {
        return false;
    }

    QMap<QString, JChannelPtr> relayers;

    foreach (auto itemJson, channelJson) {
        if (!itemJson.isObject()) {
            continue;
        }
        const std::string id = itemJson["id"].asString();
        if (id.empty()) {
            continue;
        }
        JChannelPtr newChannel = JChannelPtr(
                    new JChannel(QString::fromStdString(id), this), jdelete_qobject);
        newChannel->restore(itemJson);
        d->channels.append(newChannel);
        // relayer
        if (itemJson.isMember("relayer")) {
            const QString relayerId = QString::fromStdString(itemJson["repayer"].asString()).trimmed();
            if (!relayerId.isEmpty()) {
                relayers[relayerId] = newChannel;
            }
        }
    }

    QMapIterator<QString, JChannelPtr> citerRelayers(relayers);
    while (citerRelayers.hasNext()) {
        citerRelayers.next();
        icdmeta::JChannel *channel = identityOf(citerRelayers.key());
        if (!channel || !channel->isValid()) {
            continue;
        }
        Icd::ChannelPtr nativeChannel = channel->nativeChannel();
        if (!nativeChannel) {
            continue;
        }
        Icd::ChannelPtr nativeRelayer = citerRelayers.value()->nativeChannel();
        if (!nativeRelayer) {
            continue;
        }
        nativeRelayer->setRelayer(nativeChannel);
    }

    return true;
}

bool JChannelPool::saveConfig(const QString &filePath, const QString &nodePath)
{
    Q_D(JChannelPool);
    if (d->channels.isEmpty()) {
        return true;
    }

    Json::Value json;

    foreach (auto &channel, d->channels) {
        if (channel) {
            json.append(channel->save());
        }
    }

    return Json::make(filePath.toStdString(), nodePath.toStdString(), json, true, false);
}

JChannel *JChannelPool::identityOf(const QString &identity) const
{
    Q_D(const JChannelPool);
    for (auto &channel : d->channels) {
        if (channel->identity() == identity) {
            return channel.data();
        }
    }

    return nullptr;
}

QList<JChannel *> JChannelPool::identityOf(const QString &identity, bool fuzzy) const
{
    Q_D(const JChannelPool);
    QList<JChannel *> channels;
    for (auto &channel : d->channels) {
        if (fuzzy) {
            if (channel->identity().contains(QRegExp(identity))) {
                channels.append(channel.data());
            }
        } else {
            if (channel->identity() == identity) {
                channels.append(channel.data());
                break;
            }
        }
    }

    return channels;
}

JChannel *JChannelPool::domainOf(const QString &domain) const
{
    Q_D(const JChannelPool);
    for (auto &channel : d->channels) {
        if (channel->domain() == domain) {
            return channel.data();
        }
    }

    return nullptr;
}

void JChannelPool::reset()
{
    Q_D(JChannelPool);
    d->clearChannel();
}
