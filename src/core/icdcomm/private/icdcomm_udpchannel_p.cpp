#include "precomp.h"
#include "icdcomm_udpchannel_p.h"
#include <QNetworkConfigurationManager>
#include "jcircularbuffer.h"

namespace Icd {

UdpChannelPrivate::UdpChannelPrivate(QObject *parent)
    : QObject(parent)
    , d_socket(nullptr)
    , d_buffer(nullptr)
    , d_localHost("127.0.0.1")
    , d_localPort(8080)
    , d_remoteHost("127.0.0.1")
    , d_remotePort(8080)
    , openMode(UdpChannel::ReadWrite)
{

}

UdpChannelPrivate::~UdpChannelPrivate()
{
    reset();
}

bool UdpChannelPrivate::open()
{
    if (!isOnline()) {
        reset();
        return false;
    }

    if (isOpen()) {
        return true;
    }

    reset();
    d_socket = new QUdpSocket(this);
    connect(d_socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    if (!d_buffer) {
        d_buffer = new JCircularBuffer(20480);
    }

    // bind
    if (openMode & UdpChannel::ReadOnly) {
        //
        if (isMulticastIP(d_remoteHost)) {
            if (!d_socket->bind(QHostAddress::AnyIPv4, d_localPort)) {
                reset();
                return false;
            }
            if (!d_socket->joinMulticastGroup(d_remoteHost)) {
                reset();
                return false;
            }
        } else {
            if (!d_socket->bind(d_localHost, d_localPort)) {
                reset();
                return false;
            }
        }
    }

    return true;
}

void UdpChannelPrivate::close()
{
    if (!isOnline()) {
        reset();
        return;
    }
#if 0
    if (isOpen()) {
        reset();
        return;
    }
#endif
    reset();
}

int UdpChannelPrivate::read(char *buffer, int size)
{
    if (d_buffer) {
        return static_cast<int>(d_buffer->read(buffer, static_cast<size_t>(size)));
    } else {
        return 0;
    }
}

int UdpChannelPrivate::write(const char *buffer, int size)
{
    if (d_socket) {
        return static_cast<int>(d_socket->writeDatagram(buffer, static_cast<qint64>(size),
                                                        d_remoteHost, d_remotePort));
    } else {
        return -1;
    }
}

void UdpChannelPrivate::flush()
{
    if (isOpen()) {
        d_socket->flush();
    }
}

bool UdpChannelPrivate::isOnline() const
{
#if 0
    QNetworkConfigurationManager mgr;
    return mgr.isOnline();
#else
    return true;
#endif
}

bool UdpChannelPrivate::isOpen() const
{
    if (!isOnline()) {
        return false;
    }

    if (!d_socket || !d_buffer) {
        return false;
    }
#if 0
    if (!d_socket->isValid()) {
        return false;
    }
#endif
    if (openMode & UdpChannel::ReadOnly) {
        return (d_socket->state() == QAbstractSocket::BoundState);
    }

    return true;
}

void UdpChannelPrivate::reset()
{
    if (d_socket) {
        if (isMulticastIP(d_remoteHost)) {
            if (!d_socket->leaveMulticastGroup(d_remoteHost)) {
                //
            }
        }
        d_socket->abort();
        d_socket->disconnect(this);
        d_socket->deleteLater();
        d_socket = Q_NULLPTR;
    }
    if (d_buffer) {
        delete d_buffer;
        d_buffer = Q_NULLPTR;
    }
}

bool UdpChannelPrivate::isMulticastIP(const std::string &ip) const
{
    QHostAddress host = QHostAddress(QString::fromStdString(ip));
    return isMulticastIP(host);
}

bool UdpChannelPrivate::isMulticastIP(const QHostAddress &host) const
{
    quint32 multcastStart = QHostAddress("224.0.0.0").toIPv4Address();
    quint32 multcastStop = QHostAddress("239.255.255.255").toIPv4Address();
    quint32 nHost = host.toIPv4Address();
    if (multcastStart <= nHost && nHost <= multcastStop) {
        return true;
    } else {
        return false;
    }
}

std::string UdpChannelPrivate::localIP() const
{
    return d_localHost.toString().toStdString();
}

void UdpChannelPrivate::setLocalIP(const std::string &ip)
{
    d_localHost.setAddress(QString::fromStdString(ip));
}

int UdpChannelPrivate::localPort() const
{
    return d_localPort;
}

void UdpChannelPrivate::setLocalPort(int port)
{
    d_localPort = static_cast<quint16>(port);
}

std::string UdpChannelPrivate::remoteIP() const
{
    return d_remoteHost.toString().toStdString();
}

void UdpChannelPrivate::setRemoteIP(const std::string &ip)
{
    d_remoteHost.setAddress(QString::fromStdString(ip));
}

int UdpChannelPrivate::remotePort() const
{
    return d_remotePort;
}

void UdpChannelPrivate::setRemotePort(int port)
{
    d_remotePort = static_cast<quint16>(port);
}

int UdpChannelPrivate::lastErrorCode() const
{
    if (d_socket) {
        return static_cast<int>(d_socket->error());
    } else {
        return QUdpSocket::UnknownSocketError;
    }
}

std::string UdpChannelPrivate::lastError() const
{
    if (d_socket) {
        return d_socket->errorString().toStdString();
    } else {
        return std::string();
    }
}

int UdpChannelPrivate::sizeOfIn() const
{
    if (d_buffer) {
        return static_cast<int>(d_buffer->size());
    } else {
        return 0;
    }
}

int UdpChannelPrivate::sizeOfOut() const
{
    if (d_socket) {
        return static_cast<int>(d_socket->bytesToWrite());
    } else {
        return 0;
    }
}

bool UdpChannelPrivate::setConfig(const std::string &localIP, int localPort,
                                  const std::string &remoteIP, int remotePort)
{
    QHostAddress localHost;
    QHostAddress remoteHost;
    //
    if (!localHost.setAddress(QString::fromStdString(localIP))) {
        localHost.setAddress(QHostAddress::LocalHost);
    }
    remoteHost.setAddress(QString::fromStdString(remoteIP));
    return setConfig(localHost, static_cast<quint16>(localPort),
                     remoteHost, static_cast<quint16>(remotePort));
}

bool UdpChannelPrivate::setConfig(const QHostAddress &localHost, quint16 localPort,
                                  const QHostAddress &remoteHost, quint16 remotePort)
{
    //
    close();

    //
    if (localHost.isNull() || remoteHost.isNull()) {
        return false;
    }

    //
    d_localHost = localHost;
    d_localPort = localPort;
    d_remoteHost = remoteHost;
    d_remotePort = remotePort;

    return true;
}

void UdpChannelPrivate::onReadyRead()
{
    if (!d_socket) {
        return;
    }

    do {
        QByteArray datagram;
        datagram.resize(static_cast<int>(d_socket->pendingDatagramSize()));
        d_socket->readDatagram(datagram.data(), datagram.size());
        // write
        int index = 0, size = datagram.size();
        const char *buffer = datagram.constData();
        do {
            index += d_buffer->write(buffer + index, static_cast<size_t>(size - index));
            if (index < size) {
                qDebug() << "[1] ringbuffer overflow!!!";
                index += d_buffer->write(buffer + index, static_cast<size_t>(size - index));
                if (index < size) {
                    qDebug() << "[2] ringbuffer overflow!!!";
                    break;  // no loop
                }
            }
        } while (index < size);
    } while (d_socket->hasPendingDatagrams());
}

} // end of namespace Icd
