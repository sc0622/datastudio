#include "precomp.h"
#include "JCmdChannel.h"
#include "JCmdObject.h"
#include "../channel/JChannel.h"
#include "../channel/JSuperChannel.h"

namespace icdmeta {

// class JCmdChannelPrivate

class JCmdChannelPrivate
{
public:
    JCmdChannelPrivate(JCmdChannel *q)
        : J_QPTR(q)
        , channel(nullptr)
        , nativeChannel(nullptr)
        , cmdObject(nullptr)
        , workMode(JCmdChannel::WorkModeHome)
        , workState(JCmdChannel::WorkStateIdle)
        , workResult(JCmdChannel::WorkResultInvalid)
    {

    }

    void init();
    void success();
    void timeout();
    void cancel();
    void reset();
    bool send(const QByteArray &data);
    bool receive(bool save = true);
    bool process();

    bool startTimeout(int msecs);
    void stopTimeout();

private:
    J_DECLARE_PUBLIC(JCmdChannel)
    QMutex mutex;
    QString identity;
    JChannel *channel;
    JSuperChannel *nativeChannel;
    QSharedPointer<JCmdObject> cmdObject;
    QByteArray command;
    QString answer;
    int workMode;
    int workState;
    int workResult;
    QTimer *timerIdleRecv;
};

void JCmdChannelPrivate::init()
{
    Q_Q(JCmdChannel);
    timerIdleRecv = new QTimer(q);
    timerIdleRecv->setInterval(20);
    QObject::connect(timerIdleRecv, &QTimer::timeout, q, [=](){
        receive(false);
    });
}

void JCmdChannelPrivate::success()
{
    stopTimeout();
    mutex.lock();
    workState = JCmdChannel::WorkStateFinish;
    workResult = JCmdChannel::WorkResultSuccess;
    mutex.unlock();
    Q_Q(JCmdChannel);
    emit q->workStateChanged();
    emit q->workResultChanged();
}

void JCmdChannelPrivate::timeout()
{
    mutex.lock();
    workState = JCmdChannel::WorkStateFinish;
    workResult = JCmdChannel::WorkResultTimeout;
    mutex.unlock();
    Q_Q(JCmdChannel);
    emit q->workStateChanged();
    emit q->workResultChanged();
}

void JCmdChannelPrivate::cancel()
{
    stopTimeout();
    mutex.lock();
    command.clear();
    answer.clear();
    workState = JCmdChannel::WorkStateFinish;
    workResult = JCmdChannel::WorkResultCancel;
    mutex.unlock();
    Q_Q(JCmdChannel);
    emit q->workStateChanged();
    emit q->workResultChanged();
}

void JCmdChannelPrivate::reset()
{
    stopTimeout();
    mutex.lock();
    workState = JCmdChannel::WorkStateIdle;
    workResult = JCmdChannel::WorkResultInvalid;
    cmdObject = QSharedPointer<JCmdObject>(0);
    command.clear();
    answer.clear();
    mutex.unlock();
    Q_Q(JCmdChannel);
    emit q->workStateChanged();
    emit q->workResultChanged();
}

bool JCmdChannelPrivate::send(const QByteArray &data)
{
    if (channel) {
        if (channel->writeData(data) == data.count()) {
            return true;
        }
    }
    return false;
}

bool JCmdChannelPrivate::receive(bool save)
{
    const QByteArray data = channel->readData(4096);
    if (data.isEmpty()) {
        return false;
    }

    if (save) {
        mutex.lock();
        answer.append(data);
        mutex.unlock();
    }

    Q_Q(JCmdChannel);
    emit q->appended(data, cmdObject->suffix());

    return true;
}

bool JCmdChannelPrivate::process()
{
    mutex.lock();
    const QString answer = this->answer;
    mutex.unlock();
    const QString command = cmdObject->name();

    //
    if (command == "CPos") {
        // channel
        QString pattern = [=]() -> QString {
            QString pattern("^");
            //
            pattern.append(nativeChannel->mark());
            // workMode
            pattern.append("\\\\((MIT)|(ROU)|(FLY))?>");
            // mode-pattern
#if 0
            if (workMode == JCmdChannel::WorkModeHome) {
                pattern.append("[\\s\\S]*");    // match any string
            }
#else
            pattern.append("[\\s\\S]*");    // match any string
#endif
            //
            pattern.append("(\r)?");
            // suffix
            pattern.append(cmdObject->suffix());
            //
            pattern.append("$");
            return pattern;
        }();
        // match
        if (QRegExp(pattern).exactMatch(answer)) {
            success();
            return true;
        }
    } else if (QRegExp("((MIT)|(ROU)|(FLY))").exactMatch(command)) {
        // channel
        QString pattern = [=]() -> QString {
            QString pattern("^");
            //
            pattern.append(nativeChannel->mark());
            // workMode
            pattern.append("\\\\(").append(command).append(")>");
            //
            pattern.append("(\r)?");
            // suffix
            pattern.append(cmdObject->suffix());
            //
            pattern.append("$");
            return pattern;
        }();
        // match
        if (QRegExp(pattern).exactMatch(answer)) {
            success();
            return true;
        }
    } else {
        // channel
        QString pattern = [=]() -> QString {
            QString pattern("^");
            //
            pattern.append(nativeChannel->mark());
            // workMode
            pattern.append(JCmdChannel::workModeString(workMode).replace("\\", "\\\\"));
            // command
            pattern.append("Ans_").append(command);
            // extra - pattern
            pattern.append(cmdObject->parsedPattern());
            //
            pattern.append("(\r)?");
            // suffix
            pattern.append(cmdObject->suffix());
            //
            pattern.append("$");
            return pattern;
        }();
        // match
        switch (workMode) {
        case JCmdChannel::WorkModeRou:
        {
            if (command == "PL") {
                //
            }
            break;
        }
        default:
        {
            break;
        }}
        //
        if (QRegExp(pattern).exactMatch(answer)) {
            success();
            return true;
        }
    }

    return true;
}

bool JCmdChannelPrivate::startTimeout(int msecs)
{
    Q_Q(JCmdChannel);
    emit q->timeoutStarted(msecs);
    return true;
}

void JCmdChannelPrivate::stopTimeout()
{
    Q_Q(JCmdChannel);
    emit q->timeoutStopped();
}

// class JCmdChannel

JCmdChannel::JCmdChannel(const QString &identity, QObject *parent)
    : QObject(parent)
    , J_DPTR(new JCmdChannelPrivate(this))
{
    Q_D(JCmdChannel);
    d->identity = identity;
    d->init();
}

JCmdChannel::~JCmdChannel()
{
    Q_D(JCmdChannel);
    delete d;
}

void JCmdChannel::registerQmlType()
{
    //
    IcdMetaRegisterUncreatableType2(JCmdChannel);

    //
    JCmdObject::registerQmlType();
}

QString JCmdChannel::identity() const
{
    Q_D(const JCmdChannel);
    return d->identity;
}

icdmeta::JChannel *JCmdChannel::channel() const
{
    Q_D(const JCmdChannel);
    return d->channel;
}

icdmeta::JSuperChannel *JCmdChannel::nativeChannel() const
{
    Q_D(const JCmdChannel);
    return d->nativeChannel;
}

icdmeta::JCmdObject *JCmdChannel::command() const
{
    Q_D(const JCmdChannel);
    return d->cmdObject.data();
}

QString JCmdChannel::answer() const
{
    Q_D(const JCmdChannel);
    QMutexLocker locker(const_cast<QMutex *>(&d->mutex));
    return d->answer;
}

int JCmdChannel::workMode() const
{
    Q_D(const JCmdChannel);
    QMutexLocker locker(const_cast<QMutex *>(&d->mutex));
    return d->workMode;
}

int JCmdChannel::workState() const
{
    Q_D(const JCmdChannel);
    QMutexLocker locker(const_cast<QMutex *>(&d->mutex));
    return d->workState;
}

int JCmdChannel::workResult() const
{
    Q_D(const JCmdChannel);
    QMutexLocker locker(const_cast<QMutex *>(&d->mutex));
    return d->workResult;
}

QString JCmdChannel::workModeName(int value)
{
    switch (value) {
    case WorkModeHome:
        return tr("Disconnected");
    case WorkModeConnected:
        return tr("Connected");
    case WorkModeMit:
        return tr("MIT");
    case WorkModeRou:
        return tr("ROU");
    case WorkModeFly:
        return tr("FLY");
    default:
        return tr("Connected Error");
    }
}

QString JCmdChannel::workModeNameFromCommand(const QString &command)
{
    if (command == "MIT") {
        return workModeName(WorkModeMit);
    } else if (command == "ROU") {
        return workModeName(WorkModeRou);
    } else if (command == "ROU") {
        return workModeName(WorkModeFly);
    } else {
        return workModeName(-1);
    }
}

QString JCmdChannel::workModeNameFromProtocol(int value)
{
    int newValue = WorkModeInvalid;
    switch (value) {
    case 0: newValue = WorkModeConnected; break;
    case 1: newValue = WorkModeMit; break;
    case 2: newValue = WorkModeRou; break;
    case 3: newValue = WorkModeFly; break;
    default: break;
    }
    return workModeName(newValue);
}

QString JCmdChannel::workModeString(int value)
{
    switch (value) {
    case WorkModeHome:
        return "\\~>";
    case WorkModeConnected:
        return "\\>";
    case WorkModeMit:
        return "\\MIT>";
    case WorkModeRou:
        return "\\ROU>";
    case WorkModeFly:
        return "\\FLY>";
    default:
        return "?";
    }
}

int JCmdChannel::stringWorkMode(const QString &value, bool contains)
{
    auto match = [=](const QString &pattern) -> bool {
        if (contains) {
            return value.contains(pattern);
        } else {
            return value == pattern;
        }
    };

    if (match("\\~>")) {
        return WorkModeHome;
    } else if (match("\\>")) {
        return WorkModeConnected;
    } else if (match("\\MIT>")) {
        return WorkModeMit;
    } else if (match("\\ROU>")) {
        return WorkModeRou;
    } else {
        return WorkModeInvalid;
    }
}

void JCmdChannel::setWorkMode(int value)
{
    Q_D(JCmdChannel);
    if (value != d->workMode) {
        d->workMode = value;
        emit workModeChanged(value);
    }
}

bool JCmdChannel::binding(icdmeta::JChannel *channel)
{
    Q_D(JCmdChannel);
    if (channel != d->channel) {
        d->mutex.lock();
        d->channel = channel;
        d->nativeChannel = channel->channel();
        d->mutex.unlock();
        emit channelChanged();
        emit nativeChannelChanged();
    }

    return true;
}

void JCmdChannel::unbinding()
{
    Q_D(JCmdChannel);
    if (d->channel) {
        d->mutex.lock();
        d->reset();
        d->channel = nullptr;
        d->nativeChannel = nullptr;
        d->mutex.unlock();
        emit channelChanged();
        emit nativeChannelChanged();
    }
}

bool JCmdChannel::open()
{
    Q_D(JCmdChannel);
    if (!d->nativeChannel) {
        return false;
    }
    return d->channel->open();
}

void JCmdChannel::close()
{
    Q_D(JCmdChannel);
    if (!d->channel) {
        return;
    }
    d->reset();
    d->channel->close();
}

void JCmdChannel::timeout()
{
    Q_D(JCmdChannel);
    d->timeout();
}

void JCmdChannel::cancel()
{
    Q_D(JCmdChannel);
    d->cancel();
    emit canceled();
}

void JCmdChannel::reset()
{
    Q_D(JCmdChannel);
    d->reset();
}

bool JCmdChannel::ready(icdmeta::JCmdObject *object)
{
    if (!object || !object->isValid()) {
        return false;
    }

    Q_D(JCmdChannel);
    d->stopTimeout();
    d->mutex.lock();
    d->cmdObject = QSharedPointer<JCmdObject>(object, &QObject::deleteLater);
    QQmlEngine::setObjectOwnership(d->cmdObject.data(), QQmlEngine::CppOwnership);
    d->workState = WorkStateSend;
    d->workResult = WorkResultInvalid;
    d->command = d->cmdObject->toByteArray();
    d->answer.clear();
    d->mutex.unlock();

    emit workStateChanged();
    const QString message = d->nativeChannel->mark() + workModeString(d->workMode) + d->command;
    emit appended(message, object->suffix());

    return true;
}

void JCmdChannel::startIdleTimer()
{
    Q_D(JCmdChannel);
    if (!d->timerIdleRecv->isActive()) {
        d->timerIdleRecv->start();
    }
}

void JCmdChannel::stopIdleTimer()
{
    Q_D(JCmdChannel);
    if (d->timerIdleRecv->isActive()) {
        d->timerIdleRecv->stop();
        QCoreApplication::removePostedEvents(this, QEvent::Timer);
        while (d->timerIdleRecv->isActive()) {
            QThread::msleep(20);
        }
    }
}

void JCmdChannel::run()
{
    Q_D(JCmdChannel);
    if (!d->cmdObject || !d->channel) {
        return;
    }

    int workState = WorkStateIdle;

    while (true) {
        bool _break = false;
        d->mutex.lock();
        workState = d->workState;
        d->mutex.unlock();
        switch (workState) {
        case WorkStateSend:
        {
            d->mutex.lock();
            d->channel->writeData(d->command);
            d->workState = WorkStateRecv;
            d->mutex.unlock();
            d->startTimeout(d->cmdObject->timeout());
            break;
        }
        case WorkStateRecv:
        {
            if (!d->receive()) {
                break;
            }

            if (!d->process()) {
                break;
            }
            break;
        }
        default:
            _break = true;
            break;
        }

        if (_break) {
            break;
        }

        d->mutex.lock();
        workState = d->workState;
        d->mutex.unlock();
        if (workState == WorkStateIdle
                || workState == WorkStateFinish) {
            break;
        }

        QThread::msleep(20);
    }

    //
    d->mutex.lock();
    const int workResult = d->workResult;
    d->mutex.unlock();
    if (workResult != WorkResultTimeout) {
        d->stopTimeout();
    }
}

}
