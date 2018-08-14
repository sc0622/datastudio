#ifndef JCmdChannelSim_H
#define JCmdChannelSim_H

#include "../../icdmeta_global.h"
#include "../JCmdChannel.h"

namespace icdmeta {

class JCmdChannelSimPrivate;

class ICDMETA_EXPORT JCmdChannelSim : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString identity READ identity NOTIFY identityChanged)
    Q_PROPERTY(icdmeta::JChannel* channel READ channel NOTIFY channelChanged)
    Q_PROPERTY(icdmeta::JSuperChannel* nativeChannel READ nativeChannel NOTIFY nativeChannelChanged)
    Q_PROPERTY(int workMode READ workMode NOTIFY workModeChanged)
    Q_PROPERTY(int workState READ workState NOTIFY workStateChanged)
    Q_PROPERTY(int currentWorkMode READ currentWorkMode WRITE setCurrentWorkMode NOTIFY currentWorkModeChanged)
public:
    explicit JCmdChannelSim(const QString &identity, QObject *parent = nullptr);
    ~JCmdChannelSim();

    static void registerQmlType();

    QString identity() const;
    icdmeta::JChannel *channel() const;
    icdmeta::JSuperChannel *nativeChannel() const;
    int workMode() const;
    int workState() const;
    int currentWorkMode() const;

signals:
    void identityChanged();
    void channelChanged();
    void nativeChannelChanged();
    void workModeChanged(int value);
    void workStateChanged();
    void currentWorkModeChanged(int value);
    void canceled();

public slots:
    void setCurrentWorkMode(int value);
    bool binding(icdmeta::JChannel *channel);
    void unbinding();
    bool open();
    void close();
    void timeout();
    void cancel();
    void reset();

private:
    void setWorkMode(int value);
    void run();

private:
    Q_DISABLE_COPY(JCmdChannelSim)
    J_DECLARE_PRIVATE(JCmdChannelSim)
    friend class JCmdModelSim;
    friend class JCmdModelSimPrivate;
};

}

#endif // JCmdChannelSim_H
