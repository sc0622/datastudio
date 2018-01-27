#ifndef JDATACHANNEL_H
#define JDATACHANNEL_H

#include "../icdmeta_global.h"

namespace icdmeta {

class JIcdTable;

J_TYPEDEF_QT_SHAREDPTR(JDataChannel)

class JDataChannelPrivate;
class JChannel;
class JProtocol;

class ICDMETA_EXPORT JDataChannel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString identity READ identity NOTIFY identityChanged)
    Q_PROPERTY(icdmeta::JChannel* channel READ channel NOTIFY channelChanged)
    Q_PROPERTY(icdmeta::JProtocol* protocolSend READ protocolSend NOTIFY protocolSendChanged)
    Q_PROPERTY(icdmeta::JProtocol* protocolRecv READ protocolRecv NOTIFY protocolRecvChanged)
public:
    explicit JDataChannel(const QString &identity, QObject *parent = 0);
    ~JDataChannel();

    static void registerQmlType();

    QString identity() const;
    icdmeta::JChannel *channel() const;
    icdmeta::JProtocol *protocolSend() const;
    icdmeta::JProtocol *protocolRecv() const;

signals:
    void identityChanged();
    void channelChanged();
    void protocolSendChanged();
    void protocolRecvChanged();

public slots:
    bool binding(icdmeta::JChannel *channel, icdmeta::JProtocol *protocolSend,
                 icdmeta::JProtocol *protocolRecv);
    void unbinding();
    void unbindingSend();
    void unbindingRecv();
    void setCounterLoop(bool loop);
    void setFrameLoop(bool loop);
    bool open();
    void close();
    bool start(int intervalSend = -1, int intervalRecv = -1);
    bool startSend(int interval = -1);
    bool startRecv(int interval = -1);
    void stop();
    void stopSend();
    void stopRecv();
    bool sendOnce(bool counterLoop = false, bool frameLoop = false);
    bool sendOnce(const QByteArray &data);

private:
    Q_DISABLE_COPY(JDataChannel)
    J_DECLARE_PRIVATE(JDataChannel)
};

}

#endif // JDATACHANNEL_H
