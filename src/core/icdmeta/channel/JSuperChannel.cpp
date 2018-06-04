#include "precomp.h"
#include "JSuperChannel.h"
#include "icdcomm/icdcomm_channel.h"
#include "icdworker/icdworker_inc.h"
#include "icdcore/3rdpart/jsoncpp/json/json.h"
#include "JSerialChannel.h"
#include "JUdpChannel.h"
#include "JFileChannel.h"

namespace icdmeta {

// class JSuperChannelPrivate

class JSuperChannelPrivate
{
public:
    JSuperChannelPrivate(JSuperChannel *q)
        : J_QPTR(q)
        , channel(nullptr)
    {

    }

    void init();

private:
    J_DECLARE_PUBLIC(JSuperChannel)
    Icd::ChannelPtr channel;
    QString mark;
    QString desc;
};

void JSuperChannelPrivate::init()
{

}

}

using namespace icdmeta;

// class JSuperChannel

JSuperChannel::JSuperChannel( QObject *parent)
    : QObject(parent)
    , J_DPTR(new JSuperChannelPrivate(this))
{
    Q_D(JSuperChannel);
    d->init();
}

JSuperChannel::~JSuperChannel()
{
    Q_D(JSuperChannel);
    delete d;
}

void JSuperChannel::registerQmlType()
{
    //
    IcdMetaRegisterUncreatableType2(JSuperChannel);

    //
    JSerialChannel::registerQmlType();
    JUdpChannel::registerQmlType();
}

Icd::ChannelPtr JSuperChannel::nativeChannel() const
{
    Q_D(const JSuperChannel);
    return d->channel;
}

QStringList JSuperChannel::channelTypes() const
{
    QStringList items;
    items.append(tr("Serial Port"));
    items.append(tr("UDP Channel"));
    items.append(tr("File Channel"));
    return items;
}

void JSuperChannel::setChannel(const Icd::ChannelPtr &channel)
{
    Q_D(JSuperChannel);
    if (channel == Q_NULLPTR) {
        d->channel = Q_NULLPTR;
        return;
    }
    if (channelType() != (ChannelType)channel->channelType()) {
        return;
    }
    d->channel = channel;
}

JSuperChannel::ChannelType JSuperChannel::channelType() const
{
    return JSuperChannel::ChannelInvalid;
}

QString JSuperChannel::name() const
{
    Q_D(const JSuperChannel);
    if (d->channel) {
        return QString::fromStdString(d->channel->name());
    } else {
        return QString();
    }
}

QString JSuperChannel::mark() const
{
    Q_D(const JSuperChannel);
    return d->mark;
}

QString JSuperChannel::desc() const
{
    Q_D(const JSuperChannel);
    return d->desc;
}

bool JSuperChannel::isValid() const
{
    Q_D(const JSuperChannel);
    if (d->channel) {
        return true;
    } else {
        return false;
    }
}

bool JSuperChannel::isOpen() const
{
    Q_D(const JSuperChannel);
    if (!isValid()) {
        return false;
    }
    return d->channel->isOpen();
}

bool JSuperChannel::open()
{
    Q_D(JSuperChannel);
    if (!d->channel) {
        return false;
    }

    if (d->channel->isOpen()) {
        return true;
    }

    if (!d->channel->open()) {
        return false;
    }

    emit isOpenChanged(true);

    return true;
}

void JSuperChannel::close()
{
    Q_D(JSuperChannel);
    d->channel->close();
    emit isOpenChanged(false);
}

JSuperChannel *JSuperChannel::create(JSuperChannel::ChannelType channelType)
{
    switch (channelType) {
    case ChannelSerial: return new JSerialChannel();
    case ChannelUdp: return new JUdpChannel();
    case ChannelFile: return new JFileChannel();
    default: return nullptr;
    }
}

void JSuperChannel::release(QObject *object)
{
    if (object) {
        object->deleteLater();
    }
}

void JSuperChannel::setName(const QString &name)
{
    Q_D(JSuperChannel);
    if (d->channel == Q_NULLPTR) {
        return;
    }
    const std::string d_name = d->channel->name();
    const std::string _name = name.toStdString();
    if (_name != d_name) {
        d->channel->setName(_name);
        emit nameChanged(name);
    }
}

void JSuperChannel::setMark(const QString &mark)
{
    Q_D(JSuperChannel);
    if (mark != d->mark) {
        d->mark = mark;
        emit markChanged(mark);
    }
}

void JSuperChannel::setDesc(const QString &desc)
{
    Q_D(JSuperChannel);
    if (desc != d->desc) {
        d->desc = desc;
        emit descChanged(desc);
    }
}

QByteArray JSuperChannel::readData(int size)
{
    Q_D(JSuperChannel);
    if (d->channel) {
        int sizeOfIn = d->channel->sizeOfIn();
        if (sizeOfIn <= 0) {
            return QByteArray();
        }
        QByteArray data(qMin(size, sizeOfIn), '\0');
        int newSize = d->channel->read(data.data(), data.size());
        if (newSize < data.size()) {
            data.remove(newSize, size - newSize);
        }
        return data;
    } else {
        return QByteArray();
    }
}

int JSuperChannel::writeData(const QByteArray &data)
{
    Q_D(JSuperChannel);
    if (d->channel) {
        return d->channel->write(data.data(), data.size());
    } else {
        return 0;
    }
}

Json::Value JSuperChannel::save() const
{
    Q_D(const JSuperChannel);

    Json::Value json;

    switch (d->channel->channelType()) {
    case JSuperChannel::ChannelSerial:
        json["type"] = "serial";
        break;
    case JSuperChannel::ChannelUdp:
        json["type"] = "udp";
        break;
    case JSuperChannel::ChannelFile:
        json["type"] = "file";
        break;
    default:
        break;
    }

    json["name"] = d->channel->name();
    json["mark"] = d->mark.toStdString();
    json["desc"] = d->desc.toStdString();

    return json;
}

bool JSuperChannel::restore(const Json::Value &json, int)
{
    setMark(QString::fromStdString(json["mark"].asString()));
    setName(QString::fromStdString(json["name"].asString()));
    setDesc(QString::fromStdString(json["desc"].asString()));
    return true;
}
