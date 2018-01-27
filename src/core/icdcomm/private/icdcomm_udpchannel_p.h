#ifndef ICDCOMM_UDPCHANNEL_H
#define UDPCHANNELPRIVATE_H

#include "../icdcomm_udpchannel.h"
#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>

class JCircularBuffer;

namespace Icd {

class UdpChannelPrivate : public QObject
{
    Q_OBJECT
    friend class UdpChannel;
    friend class UdpChannelData;
public:
    explicit UdpChannelPrivate(QObject *parent = 0);
    ~UdpChannelPrivate();

    bool open();
    void close();
    int read(char *buffer, int size);
    int write(const char *buffer, int size);
    void flush();
    bool isOnline() const;
    bool isOpen() const;
    void reset();
    std::string config() const;

    bool setConfig(const std::string &localIP, int localPort,
                   const std::string &remoteIP, int remotePort);

    bool setConfig(const QHostAddress &localHost, quint16 localPort,
                   const QHostAddress &remoteHost, quint16 remotePort);

    bool isMulticastIP(const std::string &ip) const;
    bool isMulticastIP(const QHostAddress &host) const;

    std::string localIP() const;
    void setLocalIP(const std::string &ip);
    int localPort() const;
    void setLocalPort(int port);
    std::string remoteIP() const;
    void setRemoteIP(const std::string &ip);
    int remotePort() const;
    void setRemotePort(int port);
    int lastErrorCode() const;
    std::string lastError() const;

    int sizeOfIn() const;
    int sizeOfOut() const;

signals:

public slots:
    void onReadyRead();

private:
    QUdpSocket *d_socket;
    JCircularBuffer *d_buffer;
    QHostAddress d_localHost;
    quint16 d_localPort;
    QHostAddress d_remoteHost;
    quint16 d_remotePort;
    UdpChannel::OpenMode openMode;
};

} // end of namespace Icd

#endif // ICDCOMM_UDPCHANNEL_H
