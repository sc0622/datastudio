#include "precomp.h"
#include "JUdpChannel.h"
#include "icdcomm/icdcomm_udpchannel.h"
#include "icdworker/icdworker.h"
#include "icdcore/3rdpart/jsoncpp/json/json.h"

namespace icdmeta {

// class JUdpChannelPrivate

class JUdpChannelPrivate
{
public:
    JUdpChannelPrivate(JUdpChannel *q)
        : q_ptr(q)
    {

    }

    void init();
    Icd::UdpChannelPtr channel() const;

private:
    J_DECLARE_PUBLIC(JUdpChannel)
};

void JUdpChannelPrivate::init()
{
    Q_Q(JUdpChannel);
    q->setChannel(Icd::UdpChannelPtr(new Icd::UdpChannel()));
}

Icd::UdpChannelPtr JUdpChannelPrivate::channel() const
{
    Q_Q(const JUdpChannel);
    return JHandlePtrCast<Icd::UdpChannel, Icd::Channel>(q->channel());
}

}

using namespace icdmeta;

// class JUdpChannel

JUdpChannel::JUdpChannel(const QString &identity, QObject *parent)
    : JSuperChannel(identity, parent)
    , d_ptr(new JUdpChannelPrivate(this))
{
    Q_D(JUdpChannel);
    d->init();
}

JUdpChannel::~JUdpChannel()
{
    Q_D(JUdpChannel);
    delete d;
}

void JUdpChannel::registerQmlType()
{
    //
    jRegisterUncreatableType(JUdpChannel);

    //
}

JSuperChannel::ChannelType JUdpChannel::channelType() const
{
    return JSuperChannel::ChannelUdp;
}

QString JUdpChannel::localAddress() const
{
    Q_D(const JUdpChannel);
    return QString::fromStdString(d->channel()->localIP());
}

int JUdpChannel::localPort() const
{
    Q_D(const JUdpChannel);
    return d->channel()->localPort();
}

QString JUdpChannel::remoteAddress() const
{
    Q_D(const JUdpChannel);
    return QString::fromStdString(d->channel()->remoteIP());
}

int JUdpChannel::remotePort() const
{
    Q_D(const JUdpChannel);
    return d->channel()->remotePort();
}

int JUdpChannel::openMode() const
{
    Q_D(const JUdpChannel);
    return d->channel()->openMode();
}

void JUdpChannel::setLocalAddress(const QString &value)
{
    Q_D(JUdpChannel);
    if (value != QString::fromStdString(d->channel()->localIP())) {
        d->channel()->setLocalIP(value.toStdString());
        emit localAddressChanged(value);
    }
}

void JUdpChannel::setLocalPort(int value)
{
    Q_D(JUdpChannel);
    if (value != d->channel()->localPort()) {
        d->channel()->setLocalPort(value);
        emit localPortChanged(value);
    }
}

void JUdpChannel::setRemoteAddress(const QString &value)
{
    Q_D(JUdpChannel);
    if (value != QString::fromStdString(d->channel()->remoteIP())) {
        d->channel()->setRemoteIP(value.toStdString());
        emit remoteAddressChanged(value);
    }
}

void JUdpChannel::setRemotePort(int value)
{
    Q_D(JUdpChannel);
    if (value != d->channel()->remotePort()) {
        d->channel()->setRemotePort(value);
        emit remotePortChanged(value);
    }
}

void JUdpChannel::setOpenMode(int value)
{
    Q_D(JUdpChannel);
    if (value != d->channel()->openMode()) {
        d->channel()->setOpenMode((Icd::UdpChannel::OpenMode)value);
        emit openModeChanged(value);
    }
}

Json::Value JUdpChannel::save() const
{
    Json::Value json = JSuperChannel::save();

    Json::Value propertyJson;
    propertyJson["localHost"] = localAddress().toStdString();
    propertyJson["localPort"] = localPort();
    propertyJson["remoteHost"] = remoteAddress().toStdString();
    propertyJson["remotePort"] = remotePort();
    // openMode
    switch (openMode()) {
    case ReadOnly: propertyJson["openMode"] = "ReadOnly"; break;
    case WriteOnly: propertyJson["openMode"] = "WriteOnly"; break;
    case ReadWrite: propertyJson["openMode"] = "ReadWrite"; break;
    default: propertyJson["openMode"] = "ReadWrite"; break;
    }
    json["property"] = propertyJson;

    return json;
}

bool JUdpChannel::restore(const Json::Value &json, int deep)
{
    if (!JSuperChannel::restore(json, deep)) {
        return false;
    }

    Json::Value propertyJson = json["property"];
    if (propertyJson.isNull()) {
        return false;
    }

    setLocalAddress(QString::fromStdString(propertyJson["localHost"].asString()));
    setLocalPort(propertyJson["localPort"].asInt());
    setRemoteAddress(QString::fromStdString(propertyJson["remoteHost"].asString()));
    setRemotePort(propertyJson["remotePort"].asInt());
    // openMode
    const QString openMode = QString::fromStdString(propertyJson["opneMode"].asString()).toLower();
    if (openMode == "readonly") {
        setOpenMode(ReadOnly);
    } else if (openMode == "writeonly") {
        setOpenMode(WriteOnly);
    } else if (openMode == "readwrite") {
        setOpenMode(ReadWrite);
    } else {
        setOpenMode(ReadWrite);
    }

    return true;
}
