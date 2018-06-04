#include "precomp.h"
#include "JChannel.h"
#include "icdcomm/icdcomm_channel.h"
#include "icdworker/icdworker_inc.h"
#include "icdcore/3rdpart/jsoncpp/json/json.h"
#include "JSuperChannel.h"

namespace icdmeta {

// class JChannelPrivate

class JChannelPrivate
{
public:
    JChannelPrivate(JChannel *q)
        : J_QPTR(q)
        , checked(false)
        , channel(nullptr)
    {

    }

    void init();
    JSuperChannel::ChannelType parseChannelType(const QString &name);

private:
    J_DECLARE_PUBLIC(JChannel)
    QString identity;
    QString domain;
    bool checked;
    JSuperChannel *channel;
};

void JChannelPrivate::init()
{

}

JSuperChannel::ChannelType JChannelPrivate::parseChannelType(const QString &name)
{
    if (name == "serial") {
        return JSuperChannel::ChannelSerial;
    } else if (name == "udp") {
        return JSuperChannel::ChannelUdp;
    } else if (name == "file") {
        return JSuperChannel::ChannelFile;
    } else {
        return JSuperChannel::ChannelInvalid;
    }
}

}

using namespace icdmeta;

// class JChannel

JChannel::JChannel(const QString &identity, QObject *parent)
    : QObject(parent)
    , J_DPTR(new JChannelPrivate(this))
{
    Q_D(JChannel);
    d->identity = identity;
    d->init();
}

JChannel::~JChannel()
{
    Q_D(JChannel);
    delete d;
}

void JChannel::registerQmlType()
{
    IcdMetaRegisterUncreatableType2(JChannel);

    JSuperChannel::registerQmlType();
}

QString JChannel::identity() const
{
    Q_D(const JChannel);
    return d->identity;
}

QString JChannel::domain() const
{
    Q_D(const JChannel);
    return d->domain;
}

JSuperChannel *JChannel::channel() const
{
    Q_D(const JChannel);
    return d->channel;
}

Icd::ChannelPtr JChannel::nativeChannel() const
{
    Q_D(const JChannel);
    if (!d->channel) {
        return nullptr;
    }
    return d->channel->nativeChannel();
}

bool JChannel::isChecked() const
{
    Q_D(const JChannel);
    return d->checked;
}

bool JChannel::isValid() const
{
    Q_D(const JChannel);
    if (d->channel && d->channel->nativeChannel()) {
        return true;
    } else {
        return false;
    }
}

bool JChannel::isOpen() const
{
    Q_D(const JChannel);
    if (!isValid()) {
        return false;
    }
    Icd::ChannelPtr channel = d->channel->nativeChannel();
    return channel->isOpen();
}

void JChannel::setChannel(JSuperChannel *channel)
{
    Q_D(JChannel);
    if (d->channel != channel) {
        if (d->channel) {
            d->channel->disconnect(this);
            d->channel->deleteLater();
        }
        d->channel = channel;
        emit channelChanged(channel);
        if (d->channel) {
            connect(d->channel, &JSuperChannel::isOpenChanged,
                    this, &JChannel::isOpenChanged);
        }
    }
}

void JChannel::setChecked(bool checked)
{
    Q_D(JChannel);
    if (checked != d->checked) {
        d->checked = checked;
        emit checkedChanged(checked);
    }
}

QByteArray JChannel::readData(int size) const
{
    Q_D(const JChannel);
    if (d->channel) {
        return d->channel->readData(size);
    } else {
        return QByteArray();
    }
}

int JChannel::writeData(const QByteArray &data)
{
    Q_D(JChannel);
    if (d->channel) {
        return d->channel->writeData(data);
    } else {
        return 0;
    }
}

bool JChannel::open()
{
    Q_D(JChannel);
    if (d->channel) {
        return d->channel->open();
    } else {
        return false;
    }
}

void JChannel::close()
{
    Q_D(JChannel);
    if (d->channel) {
        d->channel->close();
    }
}

bool JChannel::saveConfig(const QString &filePath, const QString &pathPrefix)
{
    Q_D(JChannel);
    if (d->channel) {
        if (filePath.toLower().endsWith(".json")) {
            QString path = "[id=" + d->identity + ']';
            if (!pathPrefix.isEmpty()) {
                path.prepend(pathPrefix + '.');
            }
            return Json::make(filePath.toLocal8Bit(), path.toStdString(), save(), true, false);
        } else {
            return false;   // not supported
        }
    } else {
        return false;
    }
}

Json::Value JChannel::save() const
{
    Q_D(const JChannel);

    if (!d->channel) {
        return Json::Value();
    }

    Json::Value json = d->channel->save();
    json["id"] = d->identity.toStdString();
    json["domain"] = d->domain.toStdString();
    json["checked"] = d->checked;

    return json;
}

bool JChannel::restore(const Json::Value &json, int)
{
    Q_D(JChannel);

    const JSuperChannel::ChannelType channelType =
            d->parseChannelType(QString::fromStdString(json["type"].asString()));
    if (channelType == JSuperChannel::ChannelInvalid) {
        setChannel(nullptr);
        return false;
    }

    d->identity = QString::fromStdString(json["id"].asString());
    emit identityChanged();
    d->domain = QString::fromStdString(json["domain"].asString());
    emit domainChanged();
    setChecked(json["checked"].asBool());

    JSuperChannel *newChannel = nullptr;
    if (!d->channel || d->channel->channelType() != channelType) {
        newChannel = d->channel->create(channelType);
        if (!newChannel) {
            Q_ASSERT(false);
            setChannel(nullptr);
            return false;
        }
        newChannel->setParent(this);
    } else {
        newChannel = d->channel;
    }

    if (!newChannel) {
        setChannel(nullptr);
        return false;
    }

    newChannel->restore(json);

    setChannel(newChannel);

    return true;
}
