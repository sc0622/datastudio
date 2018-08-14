#ifndef JCMDMODELSIM_H
#define JCMDMODELSIM_H

#include "../../icdmeta_global.h"
#include "../JCmdChannel.h"

namespace icdmeta {

class JCmdModelSimPrivate;
class JCmdChannelSim;

class ICDMETA_EXPORT JCmdModelSim : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int workMode READ workMode WRITE setWorkMode NOTIFY workModeChanged)
    Q_PROPERTY(int currentWorkMode READ currentWorkMode WRITE setCurrentWorkMode NOTIFY currentWorkModeChanged)
    Q_PROPERTY(QQmlListProperty<icdmeta::JCmdChannelSim> channels READ channels NOTIFY channelsChanged)
public:
    explicit JCmdModelSim(QObject *parent = nullptr);
    ~JCmdModelSim();

    static void registerQmlType();

    int workMode() const;
    int currentWorkMode() const;
    QQmlListProperty<icdmeta::JCmdChannelSim> channels();

    Q_INVOKABLE icdmeta::JCmdChannelSim *findItem(const QString &identity) const;

signals:
    void workModeChanged(int value);
    void currentWorkModeChanged(int value);
    void channelsChanged();

public slots:
    void setWorkMode(int value);
    void setCurrentWorkMode(int value);
    void appendChannel(icdmeta::JChannel *channel);
    void clearChannel();
    bool open();
    void close();
    void reset();

private:
    Q_DISABLE_COPY(JCmdModelSim)
    J_DECLARE_PRIVATE(JCmdModelSim)
    J_DECLARE_SINGLE_INSTANCE(JCmdModelSim)
};

}

#endif // JCMDMODELSIM_H
