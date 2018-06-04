#include "precomp.h"
#include "icdcomm_udpchannel.h"
#include "private/icdcomm_udpchannel_p.h"
#include <sstream>

namespace Icd {

// UdpChannelData

class UdpChannelData
{
    friend class UdpChannel;
public:
    UdpChannelData()
        : d(new UdpChannelPrivate())
    {

    }

    static bool parseHost(const std::string &host, const std::string &type,
                          QHostAddress &hostAddress, quint16 &hostPort);
    std::string openMode() const;
    std::string openMode(const std::string &prefix) const;

private:
    std::shared_ptr<UdpChannelPrivate> d;
};

bool UdpChannelData::parseHost(const std::string &host, const std::string &type,
                               QHostAddress &hostAddress, quint16 &hostPort)
{
    // ip
    std::string::size_type offset = host.find_first_of(':');
    if (offset == std::string::npos) {
        return false;
    }
    std::string ip = host.substr(0, offset);
    // trim
    ip.erase(0, ip.find_first_not_of(' '));   // left trim
    ip.erase(ip.find_last_not_of(' ') + 1);   // right trim
    //
    if (!hostAddress.setAddress(QString::fromStdString(ip))) {
        if (type == "local") {
            hostAddress.setAddress(QHostAddress::LocalHost);
        } else {
            return false;
        }
    }
    // port
    std::string port = host.substr(offset + 1);
    // trim
    port.erase(0, port.find_first_not_of(' '));   // left trim
    port.erase(port.find_last_not_of(' ') + 1);   // right trim
    //
    int nPort = atoi(port.c_str());
    if (nPort <= 0) {
        return false;
    }
    hostPort = (quint16)nPort;

    return true;
}

std::string UdpChannelData::openMode() const
{
    return openMode("--openMode=");
}

std::string UdpChannelData::openMode(const std::string &prefix) const
{
    std::ostringstream os;
    os << prefix;
    if (d->openMode & UdpChannel::ReadOnly) {
        os << "r";
    }
    if (d->openMode & UdpChannel::WriteOnly) {
        os << "w";
    }
    return os.str();
}

// UdpChannel

UdpChannel::UdpChannel()
    : Channel(ChannelUdp)
    , d(new UdpChannelData())
{

}

UdpChannel::UdpChannel(std::string localIP, int localPort,
                       std::string remoteIP, int remotePort,
                       OpenMode openMode)
    : d(new UdpChannelData())
{
    d->d->openMode = openMode;
    d->d->setConfig(localIP, localPort, remoteIP, remotePort);
}

UdpChannel::~UdpChannel()
{

}

bool UdpChannel::open()
{
    return d->d->open();
}

void UdpChannel::close()
{
    d->d->close();
}

int UdpChannel::read(char *buffer, int size)
{
    return d->d->read(buffer, size);
}

int UdpChannel::write(const char *buffer, int size)
{
    return d->d->write(buffer, size);
}

int UdpChannel::sizeOfIn() const
{
    return d->d->sizeOfIn();
}

int UdpChannel::sizeOfOut() const
{
    return d->d->sizeOfOut();
}

void UdpChannel::flush()
{
    d->d->flush();
}

bool UdpChannel::isOpen() const
{
    return d->d->isOpen();
}

// "udp: -local [ip]:[port] -remote [ip]:[port]"
std::string UdpChannel::config() const
{
    std::ostringstream os;
    os << "udp: --local=" << d->d->d_localHost.toString().toStdString()
       << ":" << d->d->d_localPort
       << " --remote=" << d->d->d_remoteHost.toString().toStdString()
       << ":" << d->d->d_remotePort
       << " " << d->openMode()     // openmode
       << " " << Channel::config();
    return os.str();
}

// "udp: -local [ip]:[port] -remote [ip]:[port]"
bool UdpChannel::setConfig(const std::string &config)
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

    // local
    citer = items.find("local");
    if (citer == items.cend()) {
        return false;
    }
    QHostAddress localHost;
    quint16 localPort = 0;
    if (!UdpChannelData::parseHost(citer->second, "local", localHost, localPort)) {
        return false;
    }
    // remote
    citer = items.find("remote");
    if (citer == items.cend()) {
        return false;
    }
    QHostAddress remoteHost;
    quint16 remotePort = 0;
    if (!UdpChannelData::parseHost(citer->second, "remote", remoteHost, remotePort)) {
        return false;
    }

    // openMode
    citer = items.find("openMode");
    if (citer != items.cend()) {
        const std::string openMode = citer->second;
        unsigned int eOpenMode = UdpChannel::NotOpen;
        if (openMode.find('r') != std::string::npos) {
            eOpenMode |= UdpChannel::ReadOnly;
        }
        if (openMode.find('w') != std::string::npos) {
            eOpenMode |= UdpChannel::WriteOnly;
        }
        setOpenMode((OpenMode)eOpenMode);
    }

    //
    if (!d->d->setConfig(localHost, localPort, remoteHost, remotePort)) {
        return false;   //
    }

    return true;
}

std::string UdpChannel::desc() const
{
    std::ostringstream os;
    os << "L-" << d->d->d_localHost.toString().toStdString()
       << ":" << d->d->d_localPort
       << ", R-" << d->d->d_remoteHost.toString().toStdString()
       << ":" << d->d->d_remotePort
       << " " << d->openMode("-M=");
    return os.str();
}

UdpChannel::OpenMode UdpChannel::openMode() const
{
    return d->d->openMode;
}

void UdpChannel::setOpenMode(UdpChannel::OpenMode openMode)
{
    d->d->openMode = openMode;
}

bool UdpChannel::setConfig(const std::string &localIP, int localPort,
                           const std::string &remoteIP, int remotePort)
{
    return d->d->setConfig(localIP, localPort, remoteIP, remotePort);
}

bool UdpChannel::isMulticastIP(const std::string &ip) const
{
    return d->d->isMulticastIP(ip);
}

std::string UdpChannel::localIP() const
{
    return d->d->localIP();
}

void UdpChannel::setLocalIP(const std::string &ip)
{
    d->d->setLocalIP(ip);
}

int UdpChannel::localPort() const
{
    return d->d->localPort();
}

void UdpChannel::setLocalPort(int port)
{
    d->d->setLocalPort(port);
}

std::string UdpChannel::remoteIP() const
{
    return d->d->remoteIP();
}

void UdpChannel::setRemoteIP(const std::string &ip)
{
    d->d->setRemoteIP(ip);
}

int UdpChannel::remotePort() const
{
    return d->d->remotePort();
}

void UdpChannel::setRemotePort(int port)
{
    d->d->setRemotePort(port);
}

int UdpChannel::lastErrorCode() const
{
    return d->d->lastErrorCode();
}

std::string UdpChannel::lastError() const
{
    return d->d->lastError();
}

} // end of namespace Icd
