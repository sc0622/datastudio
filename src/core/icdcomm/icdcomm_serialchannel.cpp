#include "precomp.h"
#include "icdcomm_serialchannel.h"
#include <sstream>
#include <assert.h>
#include "private/jserialport.h"

namespace Icd {

// class SerialChannelData

class SerialChannelData
{
    friend class SerialChannel;
public:
    SerialChannelData()
    {

    }

private:
    JHandlePtr<JSerialPort> serialPort;
};

// class SerialChannel

SerialChannel::SerialChannel()
    : Channel(ChannelSerial)
    , d(new SerialChannelData())
{
    d->serialPort = JHandlePtr<JSerialPort>(new JSerialPort("COM1"));
    d->serialPort->setBaudRate(115200);
    d->serialPort->setDataBits(JSerialPort::Data8);
    d->serialPort->setStopBits(JSerialPort::OneStop);
    d->serialPort->setParity(JSerialPort::NoParity);
}

SerialChannel::SerialChannel(const std::string &portName, int baudRate, DataBits dataBits,
                             StopBits stopBits, Parity parity)
    : Channel(ChannelSerial)
    , d(new SerialChannelData())
{
    d->serialPort = JHandlePtr<JSerialPort>(new JSerialPort(portName));
    d->serialPort->setBaudRate(baudRate);
    d->serialPort->setDataBits((JSerialPort::DataBits)dataBits);
    d->serialPort->setStopBits((JSerialPort::StopBits)stopBits);
    d->serialPort->setParity((JSerialPort::Parity)parity);
}

SerialChannel::~SerialChannel()
{

}

std::string SerialChannel::portName() const
{
    return d->serialPort->portName();
}

void SerialChannel::setPortName(const std::string &portName)
{
    //
    d->serialPort->setPortName(portName);
}

int SerialChannel::baudRate() const
{
    return d->serialPort->baudRate();
}

void SerialChannel::setBaudRate(int baudRate)
{
    //
    d->serialPort->setBaudRate(baudRate);
}

SerialChannel::DataBits SerialChannel::dataBits() const
{
    return (SerialChannel::DataBits)d->serialPort->dataBits();
}

void SerialChannel::setDataBits(DataBits dataBits)
{
    //
    d->serialPort->setDataBits((JSerialPort::DataBits)dataBits);
}

SerialChannel::StopBits SerialChannel::stopBits() const
{
    return (SerialChannel::StopBits)d->serialPort->stopBits();
}

void SerialChannel::setStopBits(SerialChannel::StopBits stopBits)
{
    //
    d->serialPort->setStopBits((JSerialPort::StopBits)stopBits);
}

SerialChannel::Parity SerialChannel::parity() const
{
    return (SerialChannel::Parity)d->serialPort->parity();
}

void SerialChannel::setParity(SerialChannel::Parity parity)
{
    //
    d->serialPort->setParity((JSerialPort::Parity)parity);
}

int SerialChannel::lastErrorCode() const
{
    return d->serialPort->lastErrorCode();
}

std::string SerialChannel::lastError() const
{
    return d->serialPort->lastError();
}

bool SerialChannel::open()
{
    if (d->serialPort->isOpen()) {
        return true;
    } else {
        return d->serialPort->open();
    }
}

void SerialChannel::close()
{
    d->serialPort->close();
}

bool SerialChannel::waitForReadyRead(int msecs)
{
    return d->serialPort->waitForReadyRead(msecs);
}

int SerialChannel::read(char *buffer, int size)
{
    if (d->serialPort->isOpen()) {
        return d->serialPort->read(buffer, size);
    } else {
        return -1;
    }
}

int SerialChannel::write(const char *buffer, int size)
{
    if (d->serialPort->isOpen()) {
        return d->serialPort->write(buffer, size);
    } else {
        return -1;
    }
}

int SerialChannel::sizeOfIn() const
{
    return d->serialPort->sizeOfIn();
}

int SerialChannel::sizeOfOut() const
{
    return d->serialPort->sizeOfOut();
}

void SerialChannel::flush()
{
    d->serialPort->flush();
}

bool SerialChannel::isOpen() const
{
    return d->serialPort->isOpen();
}

// "serial: -n [portname] -b [baudrate] -d [datebits] -s [stopbits] -p [parity]"
std::string SerialChannel::config() const
{
    std::ostringstream os;
    os << "serial: --portName=" << d->serialPort->portName()
       << " --baudRate=" << (int)d->serialPort->baudRate()
       << " --dataBits=" << (int)d->serialPort->dataBits()
       << " --stopBits=";

    // StopBits
    switch (d->serialPort->stopBits()) {
    default:
    case JSerialPort::OneStop: os << "1"; break;
    case JSerialPort::OneAndHalfStop: os << "1.5"; break;
    case JSerialPort::TwoStop: os << "2"; break;
    }
    // Parity
    os << " --parity=";
    switch (d->serialPort->parity()) {
    case JSerialPort::NoParity:
        os << "no";
        break;
    case JSerialPort::EvenParity:
        os << "even";
        break;
    case JSerialPort::OddParity:
        os << "odd";
        break;
    case JSerialPort::SpaceParity:
        os << "space";
        break;
    case JSerialPort::MarkParity:
        os << "mark";
        break;
    default:
        os << "<?>";
        break;
    }

    os << " " << Channel::config();

    return os.str();
}

// "serial: --portName[portname] -b [baudrate] -d [datebits] -s [stopbits] -p [parity]"
bool SerialChannel::setConfig(const std::string &config)
{
    // parse
    const std::map<std::string, std::string> items = parseConfig(config);
    if (items.empty()) {
        return false;
    }

    // id
    std::map<std::string, std::string>::const_iterator citer = items.find("id");
    if (citer == items.cend()) {
        return false;
    }
    setIdentity(citer->second);

    // name
    citer = items.find("name");
    if (citer == items.cend()) {
        return false;
    }
    setName(citer->second);

    // portName
    citer = items.find("portName");
    if (citer != items.cend()) {
        setPortName(citer->second);
    }
    // baudRate
    citer = items.find("baudRate");
    if (citer != items.cend()) {
        setBaudRate(atol(citer->second.c_str()));
    }
    // dataBits
    citer = items.find("dataBits");
    if (citer != items.cend()) {
        setDataBits((DataBits)atoi(citer->second.c_str()));
    }
    // stopBits
    citer = items.find("stopBits");
    if (citer != items.cend()) {
        int nStopBots = (int)(atof(citer->second.c_str()) * 10);
        switch (nStopBots) {
        case 10:
            d->serialPort->setStopBits(JSerialPort::OneStop);
            break;
        case 15:
            d->serialPort->setStopBits(JSerialPort::OneAndHalfStop);
            break;
        case 20:
            d->serialPort->setStopBits(JSerialPort::TwoStop);
            break;
        default:
            assert(false);
            break;
        }
    }
    // parity
    citer = items.find("parity");
    if (citer != items.cend()) {
        const std::string parity = citer->second;
        if (parity == "no") {
            d->serialPort->setParity(JSerialPort::NoParity);
        } else if (parity == "even") {
            d->serialPort->setParity(JSerialPort::EvenParity);
        } else if (parity == "odd") {
            d->serialPort->setParity(JSerialPort::OddParity);
        } else if (parity == "space") {
            d->serialPort->setParity(JSerialPort::SpaceParity);
        } else if (parity == "mark") {
            d->serialPort->setParity(JSerialPort::MarkParity);
        } else {
            assert(false);
            return false;
        }
    }

    return true;
}

std::string SerialChannel::desc() const
{
    std::ostringstream os;
    std::string portName = d->serialPort->portName();
    std::transform(portName.begin(), portName.begin(), portName.end(), toupper);
    os << portName
       << '-' << (int)d->serialPort->baudRate()
       << '-' << (int)d->serialPort->dataBits()
       << '-';
    // StopBits
    switch (d->serialPort->stopBits()) {
    default:
    case JSerialPort::OneStop: os << "1"; break;
    case JSerialPort::OneAndHalfStop: os << "1.5"; break;
    case JSerialPort::TwoStop: os << "2"; break;
    }
    // Parity
    os << "-";
    switch (d->serialPort->parity()) {
    case JSerialPort::NoParity:
        os << "N";
        break;
    case JSerialPort::EvenParity:
        os << "E";
        break;
    case JSerialPort::OddParity:
        os << "O";
        break;
    case JSerialPort::SpaceParity:
        os << "S";
        break;
    case JSerialPort::MarkParity:
        os << "M";
        break;
    default:
        os << "?";
        break;
    }

    return os.str();
}

bool SerialChannel::setConfig(const std::string &portName, int baudRate,
                              DataBits dataBits, StopBits stopBits, Parity parity)
{
    //
    close();
    //
    d->serialPort->setPortName(portName);
    d->serialPort->setBaudRate(baudRate);
    d->serialPort->setDataBits((JSerialPort::DataBits)dataBits);
    d->serialPort->setStopBits((JSerialPort::StopBits)stopBits);
    d->serialPort->setParity((JSerialPort::Parity)parity);

    return true;
}

} // end of namespace Icd
