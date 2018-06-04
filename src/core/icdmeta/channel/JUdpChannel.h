#ifndef JUDPCHANNEL_H
#define JUDPCHANNEL_H

#include "JSuperChannel.h"

namespace icdmeta {

J_TYPEDEF_QT_SHAREDPTR(JUdpChannel)

class JUdpChannelPrivate;

class ICDMETA_EXPORT JUdpChannel : public JSuperChannel
{
    Q_OBJECT
    Q_PROPERTY(QString localAddress READ localAddress WRITE setLocalAddress NOTIFY localAddressChanged)
    Q_PROPERTY(int localPort READ localPort WRITE setLocalPort NOTIFY localPortChanged)
    Q_PROPERTY(QString remoteAddress READ remoteAddress WRITE setRemoteAddress NOTIFY remoteAddressChanged)
    Q_PROPERTY(int remotePort READ remotePort WRITE setRemotePort NOTIFY remotePortChanged)
    Q_PROPERTY(int openMode READ openMode WRITE setOpenMode NOTIFY openModeChanged)
public:
    enum OpenMode {
        NotOpen = 0x0000,
        ReadOnly = 0x0001,
        WriteOnly = 0x0002,
        ReadWrite = ReadOnly | WriteOnly
    };
    Q_ENUM(OpenMode)

    explicit JUdpChannel(QObject *parent = nullptr);
    ~JUdpChannel();

    static void registerQmlType();

    virtual ChannelType channelType() const;
    QString localAddress() const;
    int localPort() const;
    QString remoteAddress() const;
    int remotePort() const;
    int openMode() const;

signals:
    void localAddressChanged(const QString &value);
    void localPortChanged(int value);
    void remoteAddressChanged(const QString &value);
    void remotePortChanged(int value);
    void openModeChanged(int value);

public slots:
    void setLocalAddress(const QString &value);
    void setLocalPort(int value);
    void setRemoteAddress(const QString &value);
    void setRemotePort(int value);
    void setOpenMode(int value);

    // Serializable interface
public:
    Json::Value save() const override;
    bool restore(const Json::Value &json, int deep = -1) override;

private:
    Q_DISABLE_COPY(JUdpChannel)
    J_DECLARE_PRIVATE(JUdpChannel)
};

}

#endif // JUDPCHANNEL_H
