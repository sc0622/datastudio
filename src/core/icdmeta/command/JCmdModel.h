#ifndef JCMDMODEL_H
#define JCMDMODEL_H

#include "../icdmeta_global.h"
#include "JCmdChannel.h"

class QJSValue;

namespace icdmeta {

class JCmdModelPrivate;

class ICDMETA_EXPORT JCmdModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int workMode READ workMode WRITE setWorkMode NOTIFY workModeChanged)
    Q_PROPERTY(QQmlListProperty<icdmeta::JCmdChannel> channels READ channels NOTIFY channelsChanged)
    Q_PROPERTY(bool blocking READ blocking WRITE setBlocking NOTIFY blockingChanged)
    Q_PROPERTY(bool finished READ isFinished NOTIFY finishedChanged)
    Q_PROPERTY(icdmeta::JCmdObject* currentCommand READ currentCommand NOTIFY currentCommandChanged)
public:
    enum ExecuteError {
        SendNoValidChannel = -9,
        SendInvalidCmd = -8,
        SendInvalidParam = -7,
        SendFrequentlyFailure = -6,
        SendLowPriorityFailure = -5,
        SendSysBlockingFailure = -4,
        SendCmdBlockingFailure = -3,
        SendReadyFailure = -2,
        ExecuteArgFailure = -1
    };
    Q_ENUM(ExecuteError)

    explicit JCmdModel(QObject *parent = nullptr);
    ~JCmdModel();

    static void registerQmlType();

    int workMode() const;
    QQmlListProperty<icdmeta::JCmdChannel> channels();
    bool blocking() const;
    bool isFinished() const;
    icdmeta::JCmdObject *currentCommand() const;

    Q_INVOKABLE icdmeta::JCmdChannel *findItem(const QString &identity) const;

    Q_INVOKABLE bool send(const QString &command, QJSValue callback = QJSValue(),
                          bool force = false);  // default: sync=false
    Q_INVOKABLE bool send(const QString &command, const QString &arguments,
                          QJSValue callback = QJSValue(), bool force = false);
    Q_INVOKABLE bool send(const QString &command, bool sync, QJSValue callback = QJSValue(),
                          bool force = false);
    Q_INVOKABLE bool send(const QString &command, const QString &arguments,
                          bool sync, QJSValue callback = QJSValue(), bool force = false);
    Q_INVOKABLE bool send(const QString &command, int timeout,  QJSValue callback = QJSValue(),
                          bool force = false);  // default: sync=false
    Q_INVOKABLE bool send(const QString &command, const QString &arguments, int timeout,
                          QJSValue callback = QJSValue(), bool force = false);
    Q_INVOKABLE bool send(const QString &command, int timeout, bool sync,
                          QJSValue callback = QJSValue(), bool force = false);
    Q_INVOKABLE bool send(const QString &command, const QString &arguments, int timeout,
                          bool sync, QJSValue callback = QJSValue(), bool force = false);

    Q_INVOKABLE QString workModeName(int value) const;
    Q_INVOKABLE QString workModeNameFromCommand(const QString &command);
    Q_INVOKABLE QString workModeNameFromProtocol(int value);
    Q_INVOKABLE QString workModeString(int value) const;
    Q_INVOKABLE int stringWorkMode(const QString &value, bool contains = false) const;
    Q_INVOKABLE bool setCurrentCommandCallback(QJSValue callback, const QString &guid);
    Q_INVOKABLE void clearCurrentCommandCallback(const QString &guid);

signals:
    void workModeChanged(int value);
    void workModeStringChanged();
    void channelsChanged();
    void blockingChanged(bool value);
    void finishedChanged(bool value);
    void currentCommandChanged(icdmeta::JCmdObject *value);
    void showNotify(const QString &message, const QString &desc, const QString &guid = QString());
    void hideNotify(const QString &guid);
    void finishWatcherNotify();

public slots:
    void setWorkMode(int value);
    void setBlocking(bool value);
    void appendChannel(icdmeta::JChannel *channel);
    void clearChannel();
    bool open();
    void close();
    void finishWatcher();
    void reset();

protected:
    void customEvent(QEvent *event);

private:
    Q_DISABLE_COPY(JCmdModel)
    J_DECLARE_PRIVATE(JCmdModel)
    J_DECLARE_SINGLE_INSTANCE(JCmdModel)
};

}

#endif // JCMDMODEL_H
