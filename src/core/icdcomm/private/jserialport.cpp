#include "precomp.h"
#include "jserialport.h"
#include <sstream>

#ifdef SERIAL_USE_PCOMM
#include "moxa/pcomm.h"
#endif

//#pragma comment(lib, "pcomm.lib")

namespace Icd {

// class JSerialPortData

class JSerialPortData
{
    friend class JSerialPort;
public:
    JSerialPortData()
#ifdef SERIAL_USE_PCOMM
        : portNumber(0)
        , baudRate(B9600)
        , dataBits(BIT_8)
        , stopBits(STOP_1)
        , parity(P_NONE)
        , isOpen(false)
        , errorCode(SIO_OK)
    #else
        : serialPort(JHandlePtr<QSerialPort>(new QSerialPort()))
    #endif
    {

    }

#ifdef SERIAL_USE_PCOMM
    void setOptions();
    void setIoCtrl();
#endif

private:
#ifdef SERIAL_USE_PCOMM
    int portNumber;
    int baudRate;
    int dataBits;
    int stopBits;
    int parity;
    bool isOpen;
    int errorCode;
#else
    JHandlePtr<QSerialPort> serialPort;
#endif
};

#ifdef SERIAL_USE_PCOMM

void JSerialPortData::setOptions()
{
    int hw = false ? 0x03 : 0;  // bit0 and bit1
    int sw = false ? 0x0c : 0;  // bit2 and bit3
    bool dtr = true;
    bool rts = true;

    // set the basic options of serial port.
    setIoCtrl();

    // set the flow-control options of serial port.
    errorCode = sio_flowctrl(portNumber, hw | sw);
    if (errorCode != SIO_OK) {
        return;
    }

    // set the status of DTR (0--DTR off; 1--DTR on)
    errorCode = sio_DTR(portNumber, (dtr ? 1 : 0));
    if (errorCode != SIO_OK) {
        return;
    }

    // set the status of RTS (0-RTS off,; 1--RTS on)
    errorCode = sio_RTS(portNumber, (rts ? 1 : 0));
    if (errorCode != SIO_OK) {
        return;
    }

    // set the count of timeout to write
    long timeout = 1000 / sio_getbaud(portNumber);  // ms / bytes
    if (timeout < 1) {
        timeout = 1;
    }
    timeout *= 3;   // 1 bytes; "*3" is for delay
    if (timeout < 100) {
        timeout = 100;
    }
    //
    errorCode = sio_SetWriteTimeouts(portNumber, timeout);
}

void JSerialPortData::setIoCtrl()
{
    if (baudRate > B921600) {
        //
        errorCode = sio_ioctl(portNumber, B115200, dataBits | stopBits | parity);
        if (errorCode != SIO_OK) {
            return;
        }
        //
        errorCode = sio_baud(portNumber, baudRate);
        if (errorCode != SIO_OK) {
            return;
        }
    } else {
        //
        errorCode = sio_ioctl(portNumber, baudRate, dataBits | stopBits | parity);
        if (errorCode != SIO_OK) {
            return;
        }
    }
}

#endif

// class JSerialPort;

JSerialPort::JSerialPort()
    : d(new JSerialPortData())
{
    setPortName("COM1");
}

JSerialPort::JSerialPort(const std::string &portName)
    : d(new JSerialPortData())
{
    setPortName(portName);
}

JSerialPort::~JSerialPort()
{
    close();
}

std::string JSerialPort::portName() const
{
#ifdef SERIAL_USE_PCOMM
    std::ostringstream os;
    os << "COM" << d->portNumber;
    return os.str();
#else
    d->serialPort->portName()->toStdString();
#endif
}

void JSerialPort::setPortName(const std::string &portName)
{
#ifdef SERIAL_USE_PCOMM
    //
    if (portName.size() < 3) {
        return;
    }
    //
    bool isOpen = d->isOpen;
    if (isOpen) {
        close();
    }
    //
    std::string sPort = portName.substr(3);
    d->portNumber = ::atoi(sPort.c_str());
    //
    if (isOpen) {
        open();
    }
#else
    d->serialPort->setPortName(QString::fromStdString(portName));
#endif
}

int JSerialPort::baudRate() const
{
#ifdef SERIAL_USE_PCOMM
    switch (d->baudRate) {
    case B50: return 50;
    case B75: return 75;
    case B110: return 110;
    case B134: return 134;
    case B150: return 150;
    case B300: return 300;
    case B600: return 600;
    case B1200: return 1200;
    case B1800: return 1800;
    case B2400: return 2400;
    case B4800: return 4800;
    case B7200: return 7200;
    case B9600: return 9600;
    case B19200: return 19200;
    case B38400: return 38400;
    case B57600: return 57600;
    case B115200: return 115200;
    case B230400: return 230400;
    case B460800: return 460800;
    case B921600: return 921600;
    default: return d->baudRate;
    }
#else
    d->serialPort->baudRate();
#endif
}

void JSerialPort::setBaudRate(int value)
{
#ifdef SERIAL_USE_PCOMM
    switch (value) {
    case 50: d->baudRate = B50; break;
    case 75: d->baudRate = B75; break;
    case 110: d->baudRate = B110; break;
    case 134: d->baudRate = B134; break;
    case 150: d->baudRate = B150; break;
    case 300: d->baudRate = B300; break;
    case 600: d->baudRate = B600; break;
    case 1200: d->baudRate = B1200; break;
    case 1800: d->baudRate = B1800; break;
    case 2400: d->baudRate = B2400; break;
    case 4800: d->baudRate = B4800; break;
    case 7200: d->baudRate = B7200; break;
    case 9600: d->baudRate = B9600; break;
    case 19200: d->baudRate = B19200; break;
    case 38400: d->baudRate = B38400; break;
    case 57600: d->baudRate = B57600; break;
    case 115200: d->baudRate = B115200; break;
    case 230400: d->baudRate = B230400; break;
    case 460800: d->baudRate = B460800; break;
    case 921600: d->baudRate = B921600; break;
    default: d->baudRate = value; break;
    }
    if (d->isOpen) {
        d->errorCode = sio_baud(d->portNumber, value);
    }
#else
    d->serialPort->setBaudRate(value);
#endif
}

JSerialPort::DataBits JSerialPort::dataBits() const
{
#ifdef SERIAL_USE_PCOMM
    switch (d->dataBits) {
    case BIT_5: return JSerialPort::Data5;
    case BIT_6: return JSerialPort::Data6;
    case BIT_7: return JSerialPort::Data7;
    default:
    case BIT_8: return JSerialPort::Data8;
    }
#else
    return (SerialChannel::DataBites)d->serialPort->dataBits();
#endif
}

void JSerialPort::setDataBits(JSerialPort::DataBits value)
{
#ifdef SERIAL_USE_PCOMM
    switch (value) {
    case JSerialPort::Data5: d->dataBits = BIT_5; break;
    case JSerialPort::Data6: d->dataBits = BIT_6; break;
    case JSerialPort::Data7: d->dataBits = BIT_7; break;
    default:
    case JSerialPort::Data8: d->dataBits = BIT_8; break;
    }
    //
    if (d->isOpen) {
        d->setIoCtrl();
    }
#else
    d->serialPort->setDataBits((QSerialPort::DataBites)value);
#endif
}

JSerialPort::StopBits JSerialPort::stopBits() const
{
#ifdef SERIAL_USE_PCOMM
    switch (d->stopBits) {
    default:
    case STOP_1: return JSerialPort::OneStop;
    case STOP_2: return JSerialPort::TwoStop;
    }
#else
    return (SerialChannel::StopBits)d->serialPort->stopBits();
#endif
}

void JSerialPort::setStopBits(JSerialPort::StopBits value)
{
#ifdef SERIAL_USE_PCOMM
    switch (value) {
    default:
    case SerialChannel::OneStop: d->stopBits = STOP_1; break;
    case SerialChannel::TwoStop: d->stopBits = STOP_2; break;
    }
    //
    if (d->isOpen) {
        d->setIoCtrl();
    }
#else
    d->serialPort->setStopBits((QSerialPort::StopBits)value);
#endif
}

JSerialPort::Parity JSerialPort::parity() const
{
#ifdef SERIAL_USE_PCOMM
    switch (d->parity) {
    case P_NONE: return JSerialPort::NoParity;
    case P_EVEN: return JSerialPort::EvenParity;
    case P_ODD: return JSerialPort::OddParity;
    case P_SPC: return JSerialPort::SpaceParity;
    case P_MRK: return JSerialPort::MarkParity;
    default: return JSerialPort::InvalidParity;
    }
#else
    return (SerialChannel::Parity)d->serialPort->parity();
#endif
}

void JSerialPort::setParity(JSerialPort::Parity parity)
{
#ifdef SERIAL_USE_PCOMM
    switch (parity) {
    default:
    case JSerialPort::NoParity: d->parity = P_NONE; break;
    case JSerialPort::EvenParity: d->parity = P_EVEN; break;
    case JSerialPort::OddParity: d->parity = P_ODD; break;
    case JSerialPort::SpaceParity: d->parity = P_SPC; break;
    case JSerialPort::MarkParity: d->parity = P_MRK; break;
    }
    //
    if (d->isOpen) {
        d->setIoCtrl();
    }
#else
    d->serialPort->setStopBits((QSerialPort::Parity)parity);
#endif
}

bool JSerialPort::isOpen() const
{
#ifdef SERIAL_USE_PCOMM
    return d->isOpen;
#else
    return d->serialPort->isOpen();
#endif
}

bool JSerialPort::open()
{
#ifdef SERIAL_USE_PCOMM
    if (d->isOpen) {
        return true;
    }

    d->errorCode = sio_open(d->portNumber);
    if (SIO_OK == d->errorCode) {
        // set options
        d->setOptions();
        if (d->errorCode != SIO_OK) {
            sio_close(d->portNumber);
            return false;
        }
        d->isOpen = true;
        return true;
    } else {
        return false;
    }
#else
    if (d->serialPort->isOpen()) {
        return true;
    }

    return d->serialPort->open(QSerialPort::ReadWrite);
#endif
}

void JSerialPort::close()
{
#ifdef SERIAL_USE_PCOMM
    if (d->isOpen) {
        d->errorCode = sio_close(d->portNumber);
        if (SIO_OK == d->errorCode) {
            d->isOpen = false;
        }
    }
#else
    if (d->serialPort->isOpen()) {
        d->serialPort->close();
    }
#endif
}

bool JSerialPort::waitForReadyRead(int msecs)
{
    Q_UNUSED(msecs);
    return false;
}

int JSerialPort::read(char *buffer, int size)
{
#ifdef SERIAL_USE_PCOMM
    if (d->isOpen) {
        return sio_read(d->portNumber, buffer, size);
    } else {
        return -1;
    }
#else
    if (d->serialPort->isOpen()) {
        return d->serialPort->read(buffer, size);
    } else {
        return -1;
    }
#endif
}

int JSerialPort::write(const char *buffer, int size)
{
#ifdef SERIAL_USE_PCOMM
    if (d->isOpen) {
        return sio_write(d->portNumber, const_cast<char*>(buffer), size);
    } else {
        return -1;
    }
#else
    if (d->serialPort->isOpen()) {
        return d->serialPort->write(buffer, size);
    } else {
        return -1;
    }
#endif
}

long JSerialPort::sizeOfIn() const
{
    return sio_iqueue(d->portNumber);
}

long JSerialPort::sizeOfOut() const
{
    return sio_oqueue(d->portNumber);
}

void JSerialPort::flush()
{
    sio_flush(d->portNumber, 1);    // 0,clear input; 1,clear output; 2,clear input and output
}

int JSerialPort::lastErrorCode() const
{
#ifdef SERIAL_USE_PCOMM
    return d->errorCode;
#else
    return (int)d->serialPort->error();
#endif
}

std::string JSerialPort::lastError() const
{
#ifdef SERIAL_USE_PCOMM
    switch (d->errorCode) {
    case SIO_OK: return "successful";
    case SIO_BADPORT: return "no such port or port not opened";
    case SIO_OUTCONTROL: return "can't control the board";
    case SIO_NODATA: return "no data to read or no buffer to write";
    case SIO_OPENFAIL: return "no such port or port has be opened";
    case SIO_RTS_BY_HW: return "RTS can't set because H/W flowctrl";
    case SIO_BADPARM: return "bad parameter ";
    case SIO_WIN32FAIL: return "call win32 function fail, please call";
    case SIO_BOARDNOTSUPPORT: return "Does not support this board";
    case SIO_FAIL: return "PComm function run result fail";
    case SIO_ABORTWRITE: return "write has blocked, and user abort write";
    case SIO_WRITETIMEOUT: return "write timeoue has happened";
    default: return "";
    }
#else
    return (int)d->serialPort->errorString().toStdString();
#endif
}

} // end of namespace Icd
