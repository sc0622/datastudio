#ifndef JCHANNEL_H
#define JCHANNEL_H

#include "../icdmeta_global.h"
#include "icdcore/icdcore_global.h"

namespace Icd {
template<typename T> class JHandlePtr;
class Channel;
typedef JHandlePtr<Channel> ChannelPtr;
}

namespace icdmeta {

J_TYPEDEF_QT_SHAREDPTR(JChannel)

class JChannelPrivate;
class JSuperChannel;

class ICDMETA_EXPORT JChannel : public QObject, public Json::Serializable
{
    Q_OBJECT
    Q_PROPERTY(QString identity READ identity NOTIFY identityChanged)
    Q_PROPERTY(QString domain READ domain NOTIFY domainChanged)
    Q_PROPERTY(icdmeta::JSuperChannel* channel READ channel WRITE setChannel NOTIFY channelChanged)
    Q_PROPERTY(bool active READ isActive WRITE setActive NOTIFY activeChanged)
    Q_PROPERTY(bool valid READ isValid NOTIFY validChanged)
    Q_PROPERTY(bool isOpen READ isOpen NOTIFY isOpenChanged)
public:
    explicit JChannel(const QString &identity, QObject *parent = nullptr);
    ~JChannel();

    static void registerQmlType();

    QString identity() const;
    QString domain() const;
    icdmeta::JSuperChannel *channel() const;
    Icd::ChannelPtr nativeChannel() const;
    bool isActive() const;
    bool isValid() const;
    bool isOpen() const;

signals:
    void identityChanged();
    void domainChanged();
    void channelChanged(icdmeta::JSuperChannel *channel);
    void activeChanged(bool active);
    void validChanged(bool valid);
    void isOpenChanged(bool open);

public slots:
    void setChannel(icdmeta::JSuperChannel *channel);
    void setActive(bool active);
    QByteArray readData(int size) const;
    int writeData(const QByteArray &data);
    bool open();
    void close();

    // Serializable interface
public:
    Json::Value save() const;
    bool restore(const Json::Value &json, int /*deep*/ = -1);

private:
    Q_DISABLE_COPY(JChannel)
    J_DECLARE_PRIVATE(JChannel)
};

}

#endif // JCHANNEL_H
