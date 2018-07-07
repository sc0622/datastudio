#ifndef JDATACHANNELMGR_H
#define JDATACHANNELMGR_H

#include "../icdmeta_global.h"
#include <QQmlListProperty>

class QJSValue;

namespace icdmeta {

class JIcdTable;

// class JDataChannelMgr

class JDataChannelMgrPrivate;
class JDataChannel;
class JChannel;
class JProtocol;

class ICDMETA_EXPORT JDataChannelMgr : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<icdmeta::JDataChannel> channels READ channels NOTIFY channelsChanged)
public:
    static void registerQmlType();

    QQmlListProperty<JDataChannel> channels();

signals:
    void channelsChanged();
    void channelBound(const QString &protocolId);
    void channelUnbound(const QString &protocolId);
    void searchFinished(const QVariant &dict);

public slots:
    icdmeta::JDataChannel *binding(icdmeta::JChannel *channel, icdmeta::JProtocol *protocolSend,
                                   icdmeta::JProtocol *protocolRecv);
    icdmeta::JDataChannel *bindingRecv(icdmeta::JChannel *channel, icdmeta::JProtocol *protocol);
    void unbinding(const QString &channelId);
    icdmeta::JDataChannel *channelIdOf(const QString &channelId) const;
    void cancelWatcher();
    bool search(icdmeta::JIcdTable *table, QObject *target, QJSValue callback);
    void cancelSearch(QObject *target);
    void cancelSearch(const QList<QObject *> targets);
    void resetTarget(QObject *target);
    void clearBinding();
    void reset();

protected:
    void customEvent(QEvent *event);

private:
    explicit JDataChannelMgr(QObject *parent = nullptr);
    ~JDataChannelMgr();

private:
    Q_DISABLE_COPY(JDataChannelMgr)
    J_DECLARE_PRIVATE(JDataChannelMgr)
    J_DECLARE_SINGLE_INSTANCE(JDataChannelMgr)
};

}

#endif // JDATACHANNELMGR_H
