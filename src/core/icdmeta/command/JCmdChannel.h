#ifndef JCMDCHANNEL_H
#define JCMDCHANNEL_H

#include "../icdmeta_global.h"

namespace icdmeta {

class JChannel;
class JSuperChannel;

// class JCmdChannel

class JCmdChannelPrivate;
class JCmdObject;

class ICDMETA_EXPORT JCmdChannel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString identity READ identity NOTIFY identityChanged)
    Q_PROPERTY(icdmeta::JChannel* channel READ channel NOTIFY channelChanged)
    Q_PROPERTY(icdmeta::JSuperChannel* nativeChannel READ nativeChannel NOTIFY nativeChannelChanged)
    Q_PROPERTY(icdmeta::JCmdObject* command READ command NOTIFY commandChanged)
    Q_PROPERTY(QString answer READ answer NOTIFY answerChanged)
    Q_PROPERTY(int workMode READ workMode NOTIFY workModeChanged)
    Q_PROPERTY(int workState READ workState NOTIFY workStateChanged)
    Q_PROPERTY(int workResult READ workResult NOTIFY workResultChanged)
public:
    /**
     * @brief The WorkMode enum
     */
    enum WorkMode {
        WorkModeInvalid = 0x00,
        WorkModeHome = 0x01,
        WorkModeConnected = 0x02,
        WorkModeMit = WorkModeConnected | 0x04,
        WorkModeRou = WorkModeConnected | 0x08,
        WorkModeFly = WorkModeConnected | 0x10,
        WorkModeMask = 0x1f
    };
    Q_ENUM(WorkMode)
    Q_ENUMS(WorkModes)

    /**
     * @brief The WorkState enum
     */
    enum WorkState {
        WorkStateIdle,
        WorkStateSend,
        WorkStateRecv,
        WorkStateFinish
    };
    Q_ENUM(WorkState)

    /**
     * @brief The WorkResult enum
     */
    enum WorkResult {
        WorkResultInvalid = -3,
        WorkResultTimeout = -2,
        WorkResultCancel = -1,
        WorkResultSuccess,
    };
    Q_ENUM(WorkResult)

    explicit JCmdChannel(const QString &identity, QObject *parent = nullptr);
    ~JCmdChannel();

    static void registerQmlType();

    QString identity() const;
    icdmeta::JChannel *channel() const;
    icdmeta::JSuperChannel *nativeChannel() const;
    icdmeta::JCmdObject *command() const;
    QString answer() const;
    int workMode() const;
    int workState() const;
    int workResult() const;

    static QString workModeName(int value);
    static QString workModeNameFromCommand(const QString &command);
    static QString workModeNameFromProtocol(int value);
    static QString workModeString(int value);
    static int stringWorkMode(const QString &value, bool contains = false);

signals:
    void identityChanged();
    void channelChanged();
    void nativeChannelChanged();
    void commandChanged();
    void answerChanged();
    void workModeChanged(int value);
    void workStateChanged();
    void workResultChanged();
    void appended(const QString &msg, const QString &suffix);
    void canceled();
    void timeoutStarted(int msecs);
    void timeoutStopped();

public slots:
    bool binding(icdmeta::JChannel *channel);
    void unbinding();
    bool open();
    void close();
    void timeout();
    void cancel();
    void reset();
    bool ready(icdmeta::JCmdObject *object);

private slots:
    void startIdleTimer();
    void stopIdleTimer();

private:
    void setWorkMode(int value);
    void run();

private:
    Q_DISABLE_COPY(JCmdChannel)
    J_DECLARE_PRIVATE(JCmdChannel)
    friend class JCmdModel;
    friend class JCmdModelPrivate;
};

}

#endif // JCMDCHANNEL_H
