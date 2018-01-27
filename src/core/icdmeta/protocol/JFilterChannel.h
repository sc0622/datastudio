#ifndef JFILTERCHANNEL_H
#define JFILTERCHANNEL_H

#include "../icdmeta_global.h"

namespace icdmeta {

J_TYPEDEF_QT_SHAREDPTR(JFilterChannel)

class JFilterChannelPrivate;
class JDataChannel;
class JChannel;
class JProtocol;

class ICDMETA_EXPORT JFilterChannel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(JProtocol* protocol READ protocol WRITE setProtocol NOTIFY protocolChanged)
    Q_PROPERTY(QQmlListProperty<icdmeta::JChannel> channels READ channels NOTIFY channelsChanged)
    Q_PROPERTY(icdmeta::JChannel* currentChannel READ currentChannel NOTIFY currentChannelChanged)
    Q_PROPERTY(bool running READ isRunning NOTIFY runningChanged)
    Q_PROPERTY(bool active READ isActive NOTIFY activeChanged)
public:
    static void registerQmlType();

    icdmeta::JProtocol *protocol() const;
    QQmlListProperty<icdmeta::JChannel> channels();
    icdmeta::JChannel *currentChannel() const;
    bool isRunning() const;
    bool isActive() const;

signals:
    void protocolChanged(icdmeta::JProtocol *value);
    void channelsChanged();
    void currentChannelChanged(icdmeta::JChannel *value);
    void runningChanged();
    void activeChanged();
    void started();
    void stopped();

public slots:
    void reset();
    void setProtocol(icdmeta::JProtocol *value);
    void setCurrentChannel(const QString &mark);
    bool setCurrentChannel(icdmeta::JChannel *value);
    void appendChannel(icdmeta::JChannel *channel);
    void removeChannel(const QString &identity);
    void clearChannel();
    bool start();
    void stop();

private:
    explicit JFilterChannel(QObject *parent = 0);
    ~JFilterChannel();

private:
    Q_DISABLE_COPY(JFilterChannel)
    J_DECLARE_PRIVATE(JFilterChannel)
    J_DECLARE_SINGLE_INSTANCE(JFilterChannel)
};

}

#endif // JFILTERCHANNEL_H
