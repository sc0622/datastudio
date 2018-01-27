#include "precomp.h"
#include "JSerialChannel.h"
#include <QSerialPortInfo>
#include "icdcomm/icdcomm_serialchannel.h"
#include "icdworker/icdworker.h"
#include "icdcore/3rdpart/jsoncpp/json/json.h"

namespace icdmeta {

// class JSerialChannelPrivate

class JSerialChannelPrivate
{
public:
    JSerialChannelPrivate(JSerialChannel *q)
        : q_ptr(q)
    {

    }

    void init();
    Icd::SerialChannelPtr channel() const;

private:
    J_DECLARE_PUBLIC(JSerialChannel)
};

void JSerialChannelPrivate::init()
{
    Q_Q(JSerialChannel);
    q->setChannel(Icd::SerialChannelPtr(new Icd::SerialChannel()));
}

Icd::SerialChannelPtr JSerialChannelPrivate::channel() const
{
    Q_Q(const JSerialChannel);
    return JHandlePtrCast<Icd::SerialChannel, Icd::Channel>(q->channel());
}

}

using namespace icdmeta;

// class JSerialChannel

JSerialChannel::JSerialChannel(const QString &identity, QObject *parent)
    : JSuperChannel(identity, parent)
    , d_ptr(new JSerialChannelPrivate(this))
{
    Q_D(JSerialChannel);
    d->init();
}

JSerialChannel::~JSerialChannel()
{
    Q_D(JSerialChannel);
    delete d;
}

void JSerialChannel::registerQmlType()
{
    //
    jRegisterUncreatableType(JSerialChannel);

    //
}

JSuperChannel::ChannelType JSerialChannel::channelType() const
{
    return JSuperChannel::ChannelSerial;
}

QStringList JSerialChannel::portNames() const
{
    QStringList portNames;
    QListIterator<QSerialPortInfo> citerPorts(QSerialPortInfo::availablePorts());
    while (citerPorts.hasNext()) {
        const QSerialPortInfo &portInfo = citerPorts.next();
        portNames.append(portInfo.portName());
    }
    //
    qSort(portNames.begin(), portNames.end(),
          [](const QString &first, const QString &second){
        return first.mid(3).toInt() < second.mid(3).toInt();    // sort by number
    });
    return portNames;
}

QStringList JSerialChannel::baudRates() const
{
    QStringList baudRates;
    QListIterator<qint32> citerBaudRates(QSerialPortInfo::standardBaudRates());
    while (citerBaudRates.hasNext()) {
        const qint32 baudRate = citerBaudRates.next();
        baudRates.append(QString::number(baudRate));
    }
    return baudRates;
}

QStringList JSerialChannel::dataBitss() const
{
    QStringList dataBitss;
    for (int i = 5; i < 9; ++i) {
        dataBitss.append(QString::number(i));
    }
    return dataBitss;
}

QStringList JSerialChannel::stopBitss() const
{
    QStringList stopBitss;
    stopBitss.append("1");
    //stopBitss.append("1.5");  // not supported
    stopBitss.append("2");
    return stopBitss;
}

QStringList JSerialChannel::parities() const
{
    QStringList parities;
    parities.append(QStringLiteral("None"));
    parities.append(QStringLiteral("Odd"));
    parities.append(QStringLiteral("Even"));
    parities.append(QStringLiteral("Space"));
    parities.append(QStringLiteral("Mark"));
    return parities;
}

QString JSerialChannel::portName() const
{
    Q_D(const JSerialChannel);
    return QString::fromStdString(d->channel()->portName());
}

int JSerialChannel::baudRate() const
{
    Q_D(const JSerialChannel);
    return d->channel()->baudRate();
}

JSerialChannel::DataBits JSerialChannel::dataBits() const
{
    Q_D(const JSerialChannel);
    return (JSerialChannel::DataBits)d->channel()->dataBits();
}

JSerialChannel::StopBits JSerialChannel::stopBits() const
{
    Q_D(const JSerialChannel);
    return (JSerialChannel::StopBits)d->channel()->stopBits();
}

JSerialChannel::Parity JSerialChannel::parity() const
{
    Q_D(const JSerialChannel);
    return (JSerialChannel::Parity)d->channel()->parity();
}

void JSerialChannel::setPortName(const QString &portName)
{
    Q_D(JSerialChannel);
    Icd::SerialChannelPtr channel = d->channel();
    const std::string _portName = portName.toStdString();
    if (_portName != channel->portName()) {
        channel->setPortName(_portName);
        emit portNameChanged(portName);
    }
}

void JSerialChannel::setBaudRate(int baudRate)
{
    Q_D(JSerialChannel);
    Icd::SerialChannelPtr channel = d->channel();
    if (baudRate != channel->baudRate()) {
        channel->setBaudRate(baudRate);
        emit baudRateChanged(baudRate);
    }
}

void JSerialChannel::setDataBits(JSerialChannel::DataBits dataBits)
{
    Q_D(JSerialChannel);
    Icd::SerialChannelPtr channel = d->channel();
    if (dataBits != channel->dataBits()) {
        channel->setDataBits((Icd::SerialChannel::DataBits)dataBits);
        emit dataBitsChanged(dataBits);
    }
}

void JSerialChannel::setStopBits(JSerialChannel::StopBits stopBits)
{
    Q_D(JSerialChannel);
    Icd::SerialChannelPtr channel = d->channel();
    if (stopBits != channel->stopBits()) {
        channel->setStopBits((Icd::SerialChannel::StopBits)stopBits);
        emit stopBitsChanged(stopBits);
    }
}

void JSerialChannel::setParity(JSerialChannel::Parity parity)
{
    Q_D(JSerialChannel);
    Icd::SerialChannelPtr channel = d->channel();
    if (parity != channel->parity()) {
        channel->setParity((Icd::SerialChannel::Parity)parity);
        emit parityChanged(parity);
    }
}

Json::Value JSerialChannel::save() const
{
    Json::Value json = JSuperChannel::save();

    Json::Value propertyJson;
    propertyJson["portName"] = portName().toStdString();
    propertyJson["baudRate"] = baudRate();
    // dataBits
    switch (dataBits()) {
    case Data5: propertyJson["dataBits"] = 5; break;
    case Data6: propertyJson["dataBits"] = 6; break;
    case Data7: propertyJson["dataBits"] = 7; break;
    case Data8: propertyJson["dataBits"] = 8; break;
    default:
        break;
    }
    // stopBits
    switch (stopBits()) {
    case OneStop: propertyJson["stopBits"] = 1; break;
    case OneAndHalfStop: propertyJson["stopBits"] = 1.5; break;
    case TwoStop: propertyJson["stopBits"] = 2; break;
    default:
        break;
    }
    // parity
    switch (parity()) {
    case NoParity: propertyJson["parity"] = "None"; break;
    case EvenParity: propertyJson["parity"] = "Even"; break;
    case OddParity: propertyJson["parity"] = "Odd"; break;
    case SpaceParity: propertyJson["parity"] = "Space"; break;
    case MarkParity: propertyJson["parity"] = "Mark"; break;
    default:
        break;
    }
    json["property"] = propertyJson;

    return json;
}

bool JSerialChannel::restore(const Json::Value &json, int deep)
{
    if (!JSuperChannel::restore(json, deep)) {
        return false;
    }

    Json::Value propertyJson = json["property"];
    if (propertyJson.isNull()) {
        return false;
    }

    setPortName(QString::fromStdString(propertyJson["portName"].asString()));
    setBaudRate(propertyJson["baudRate"].asInt());
    // dataBits
    switch (propertyJson["dataBits"].asInt()) {
    case 5: setDataBits(Data5); break;
    case 6: setDataBits(Data6); break;
    case 7: setDataBits(Data7); break;
    case 8: setDataBits(Data8); break;
    default:
        break;
    }
    // stopBits
    switch (int(propertyJson["stopBits"].asDouble() * 10)) {
    case 10: setStopBits(OneStop); break;
    case 15: setStopBits(OneAndHalfStop); break;
    case 20: setStopBits(TwoStop); break;
    default:
        break;
    }
    // parity
    const QString parity = QString::fromStdString(propertyJson["parity"].asString()).toLower();
    if (parity == "none") {
        setParity(NoParity);
    } else if (parity == "even") {
        setParity(EvenParity);
    } else if (parity == "odd") {
        setParity(OddParity);
    } else if (parity == "space") {
        setParity(SpaceParity);
    } else if (parity == "mark") {
        setParity(MarkParity);
    } else {
        setParity(NoParity);
    }

    return true;
}
