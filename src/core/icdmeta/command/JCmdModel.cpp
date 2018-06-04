#include "precomp.h"
#include "JCmdModel.h"
#include "JCmdObject.h"
#include "JCmdChannel.h"
#include "../channel/JChannel.h"
#include "../channel/JSuperChannel.h"
#include "simulate/JCmdSysModelSim.h"

#define DEF_ENABLED_TIMER_FREQ  1

namespace icdmeta {

// class JCmdModelPrivate

class JCmdModelPrivate
{
public:
    JCmdModelPrivate(JCmdModel *q)
        : J_QPTR(q)
        , workMode(JCmdChannel::WorkModeHome)
        , blocking(false)
    {

    }

    void init();
    bool loadConfig();
    static int channelsCount(QQmlListProperty<JCmdChannel> *property);
    static JCmdChannel *channelsAt(QQmlListProperty<JCmdChannel> *property, int index);

    static QSharedPointer<JCmdChannel> mapTask(const QSharedPointer<JCmdChannel> &channel);
    static void mapReduce(int &count, const QSharedPointer<JCmdChannel> &channel);

    QSharedPointer<JCmdObject> findCommand(const QString &command);
    QSharedPointer<JCmdObject> findCommand(const QString &command, int workMode);
    bool send(const QSharedPointer<JCmdObject> &object, QJSValue callback, bool force);
    void showNotifyError(const QString &command, QJSValue callback, JCmdModel::ExecuteError flag,
                         const QSharedPointer<JCmdObject> &object = QSharedPointer<JCmdObject>());
    void finishWatcher(bool flush = false);
    void reset();

    template<typename T>
    static void replaceValue(Json::Value &value, const std::string &key, const T &defaultValue);

private:
    J_DECLARE_PUBLIC(JCmdModel)
    int workMode;
    QMap<int, QList<QSharedPointer<JCmdObject> > > commands;
    QList<QSharedPointer<JCmdChannel> > channels;
    QFutureWatcher<int> watcherSync;
    QFutureWatcher<QSharedPointer<JCmdChannel> > watcherAsync;
    QJSValue callback;
    QSharedPointer<JCmdObject> currentCommand;
#if DEF_ENABLED_TIMER_FREQ
    QTimer *timerFreq;
#endif
    bool blocking;
};

void JCmdModelPrivate::init()
{
#if DEF_ENABLED_TIMER_FREQ
    Q_Q(JCmdModel);
    timerFreq = new QTimer(q);
    timerFreq->setInterval(100);    // ? ms
    timerFreq->setSingleShot(true);
    QObject::connect(timerFreq, &QTimer::timeout, q, [=](){
        const QVariant guid = timerFreq->property("guid");
        if (guid.isValid()) {
            q->hideNotify(guid.toString());
        }
    });
#endif
    //
    if (!loadConfig()) {
        //
    }
}

bool JCmdModelPrivate::loadConfig()
{
    QSettings settings;
    settings.beginGroup(QCoreApplication::applicationVersion().append("/Temp/Paths"));
    const QString filePath = settings.value("CONFIG_FILE").toString();
    settings.endGroup();

    const Json::Value commandJson = Json::resolve(filePath.toLocal8Bit(), "command");
    if (commandJson.isNull()) {
        return false;
    }

    // suffix
    const std::string suffix = commandJson["suffix"].asString();

    const Json::Value defaultJson = commandJson["default"];

    // default - pattern
    const std::string defaultPattern = defaultJson["pattern"].asString();
    // default - sync
    const bool defaultSync = defaultJson["sync"].asBool();
    // default - timeout
    const int defaultTimeout = defaultJson["timeout"].asInt();
    // default - message
    const std::string defaultMessage = defaultJson["message"].asString();
    // default - desc
    const std::string defaultDesc = defaultJson["desc"].asString();
    // default - blocking
    const bool defaultBlocking = defaultJson["blocking"].asBool();
    // default - priority
    const int defaultPriority = defaultJson["priority"].asInt();
    // default - acceptCount
    const int defaultAcceptCount = defaultJson["acceptCount"].asInt();

    Q_Q(JCmdModel);

    auto parseCommand = [=,this](const Json::Value &commandJson, int workMode) {
        if (commandJson.isNull()) {
            return;
        }
        this->commands.insert(workMode, QList<QSharedPointer<JCmdObject> >());
        QList<QSharedPointer<JCmdObject> > &objects = this->commands[workMode];
        //
        for (Json::ValueConstIterator citer = commandJson.begin(); citer != commandJson.end(); ++citer) {
            Json::Value itemJson = *citer;
            if (!itemJson.isObject()) {
                continue;
            }
            if (!itemJson.isMember("name")) {
                continue;
            }
            // pattern
            replaceValue<std::string>(itemJson, "pattern", defaultPattern);
            // sync
            replaceValue<bool>(itemJson, "sync", defaultSync);
            // timeout
            replaceValue<int>(itemJson, "timeout", defaultTimeout);
            // message
            replaceValue<std::string>(itemJson, "message", defaultMessage);
            // desc
            replaceValue<std::string>(itemJson, "desc", defaultDesc);
            // blocking
            replaceValue<bool>(itemJson, "blocking", defaultBlocking);
            // priority
            replaceValue<int>(itemJson, "priority", defaultPriority);
            // acceptCount
            replaceValue<int>(itemJson, "acceptCount", defaultAcceptCount);
            //
            QSharedPointer<JCmdObject> newObject(new JCmdObject(q), jdelete_qobject);
            newObject->setSuffix(QString::fromStdString(suffix));
            newObject->restore(itemJson);
            objects.append(newObject);
        }
    };

    // global
    parseCommand(commandJson["global"], JCmdChannel::WorkModeHome);
    // conn
    parseCommand(commandJson["conn"], JCmdChannel::WorkModeConnected);
    // mit
    parseCommand(commandJson["mit"], JCmdChannel::WorkModeMit);
    // rou
    parseCommand(commandJson["rou"], JCmdChannel::WorkModeRou);

    return true;
}

int JCmdModelPrivate::channelsCount(QQmlListProperty<JCmdChannel> *property)
{
    JCmdModel *q = qobject_cast<JCmdModel *>(property->object);
    return q->J_DPTR->channels.count();
}

JCmdChannel *JCmdModelPrivate::channelsAt(QQmlListProperty<JCmdChannel> *property, int index)
{
    JCmdModel *q = qobject_cast<JCmdModel *>(property->object);
    return q->J_DPTR->channels[index].data();
}

QSharedPointer<JCmdChannel> JCmdModelPrivate::mapTask(const QSharedPointer<JCmdChannel> &channel)
{
    channel->run();
    return channel;
}

void JCmdModelPrivate::mapReduce(int &count, const QSharedPointer<JCmdChannel> &channel)
{
    if (channel->workResult() == JCmdChannel::WorkResultSuccess) {
        ++count;
    }
}

QSharedPointer<JCmdObject> JCmdModelPrivate::findCommand(const QString &command)
{
    auto find = [=]() -> QSharedPointer<icdmeta::JCmdObject> {
        if (command.isEmpty()) {
            return QSharedPointer<icdmeta::JCmdObject>();
        }

        for (const auto &objects : commands) {
            for (const auto &object : objects) {
                if (object->name() == command) {
                    return QSharedPointer<JCmdObject>(object->clone(), &QObject::deleteLater);
                }
            }
        }

        return QSharedPointer<JCmdObject>();
    };

    auto object = find();
    if (!object) {
        showNotifyError(command, callback, JCmdModel::SendInvalidCmd);
    }

    return object;
}

QSharedPointer<JCmdObject> JCmdModelPrivate::findCommand(const QString &command,
                                                         int workMode)
{
    auto find = [=]() -> QSharedPointer<JCmdObject> {
        if (command.isEmpty()) {
            return QSharedPointer<JCmdObject>();
        }

        auto findByWorkMode = [=](int _workMode) -> QSharedPointer<JCmdObject> {
            auto citer = commands.find(_workMode);
            if (citer != commands.constEnd()) {
                const auto &objects = citer.value();
                for (const auto &object : objects) {
                    if (object->name() == command) {
                        return QSharedPointer<JCmdObject>(object->clone(), &QObject::deleteLater);
                    }
                }
            }
            return QSharedPointer<JCmdObject>();
        };

        auto object = findByWorkMode(JCmdChannel::WorkModeHome);
        if (object) {
            return object;
        }

        return findByWorkMode(workMode);
    };

    auto object = find();
    if (!object) {
        showNotifyError(command, callback, JCmdModel::SendInvalidCmd);
    }

    return object;
}

bool JCmdModelPrivate::send(const QSharedPointer<JCmdObject> &object, QJSValue callback, bool force)
{
    if (channels.isEmpty()) {
        showNotifyError(QString(), callback, JCmdModel::SendNoValidChannel, Q_NULLPTR);
        return false;
    }

    if (!object) {
        showNotifyError(QString(), callback, JCmdModel::SendInvalidParam, Q_NULLPTR);
        return false;
    }

    if (!force) {
        if (blocking) {
            showNotifyError(object->desc(), callback, JCmdModel::SendSysBlockingFailure, object);
            return false;
        } else {
            if (currentCommand && currentCommand->blocking()) {
                showNotifyError(object->desc(), callback, JCmdModel::SendCmdBlockingFailure, currentCommand);
                return false;
            }

            if (currentCommand && currentCommand->priority() >= object->priority()) {
                showNotifyError(object->desc(), callback, JCmdModel::SendLowPriorityFailure, object);
                return false;
            }
        }
    }
#if DEF_ENABLED_TIMER_FREQ
    if (timerFreq->isActive()) {
        showNotifyError(object->desc(), callback, JCmdModel::SendFrequentlyFailure, object);
        return false;
    }
#endif
#if DEF_ENABLED_TIMER_FREQ
    timerFreq->start();
#endif
    finishWatcher(true);

    // parse pattern of command object
    object->parsePattern();

    for (auto channel : channels) {
        JCmdObject *newObject = object->clone();
        if (channel->ready(newObject)) {
            channel->stopIdleTimer();
        } else {
            newObject->deleteLater();
            showNotifyError(object->desc(), callback, JCmdModel::SendReadyFailure);
            return false;
        }
    }

    //
    this->callback = callback;
    currentCommand = object;

    QFuture<int> futureSync;
    QFuture<QSharedPointer<JCmdChannel> > futureAsync;

    if (object->isSync()) {
        futureSync = QtConcurrent::mappedReduced(channels, mapTask, mapReduce);
    } else {
        futureAsync = QtConcurrent::mapped(channels, mapTask);
    }

    Q_Q(JCmdModel);

    auto reduce = [=,this](){
        if (!currentCommand) {
            return;
        }
        const bool sync = currentCommand->isSync();
        const int acceptCount = currentCommand->acceptCount();
        const bool isCancel = [=](){
            if (sync) {
                return watcherSync.isCanceled();
            } else {
                return watcherAsync.isCanceled();
            }
        }();
        const int count = [=]() -> int {
            if (isCancel) {
                return 0;
            }
            int count = 0;
            if (sync) {
                count = watcherSync.future().result();
            } else {
                for (auto channel : channels) {
                    if (channel->workResult() == JCmdChannel::WorkResultSuccess) {
                        ++count;
                    }
                }
            }
            return count;
        }();
        //
        const bool finished = [=]() -> bool {
            if (isCancel) {
                return true;
            }
            if (sync) {
                return true;
            } else {
                if (watcherAsync.isFinished()) {
                    return true;
                } else {
                    return (count >= acceptCount);
                }
            }
        }();
        //
        if (!finished) {
            return;
        }
        // reset
        const QString message = currentCommand->message();
        const QString desc = currentCommand->desc();
        const QString suffix = currentCommand->suffix();
        currentCommand->callWithClear();    //
        currentCommand = nullptr;
        if (sync) {
            for (auto channel : channels) {
                channel->startIdleTimer();
            }
        } else {
            watcherAsync.disconnect(q);
        }
        //
#if DEF_ENABLED_TIMER_FREQ
        timerFreq->stop();
#endif
        //
        if (!this->callback.isCallable()) {
            return;
        }

        //
        QJSValue _callback = this->callback;
        this->callback = QJSValue();

        // show notify
        bool success = [=]() -> bool {
            if (isCancel) {
                return false;
            }
            if (sync) {
                return (count == channels.count());
            } else {
                return (count >= acceptCount);
            }
        }();
        // answer
        const QString answer = [=]() -> QString {
            if (isCancel) {
                return false;
            }
            QString answer;
            for (const auto channel : channels) {
                if (channel->workState() != JCmdChannel::WorkStateFinish) {
                    continue;
                }
                QString _answer = channel->answer();
                if (success) {
                    if (channel->workResult() == JCmdChannel::WorkResultSuccess) {
                        answer.append(_answer.mid(1));    // remove channel name
                        break;
                    }
                } else {
                    if (!_answer.isEmpty()) {
                        answer.append("<font size=4 color=#51dd52>")
                                .append(channel->nativeChannel()->mark()).append(": </font><br/>")
                                .append(_answer.replace(suffix, "<br/>").replace("\r", "<br/>"))
                                .append("<br/>");
                    }
                }
            }
            return answer;
        }();
        // notify
        if (!isCancel && !success) {
            if (answer.isEmpty()) {
                emit q->showNotify(message, desc);
            } else {
                emit q->showNotify(JCmdModel::tr("Answer error!"), answer);
            }
        }
        //
        const QString answerArgs = [=]() -> QString {
            if (answer.isEmpty()) {
                return "";
            } else {
                QString answerArgs = answer.section('#', 1);
                answerArgs.chop(suffix.count());
                while (answerArgs.endsWith("\r")) {
                    answerArgs.chop(1);
                }
                return answerArgs;
            }
        }();
        if (!isCancel) {
            if (QRegExp("[\\s\\S]*[#_](FALSE)[\\s\\S]*").exactMatch(answerArgs)) {
                success = false;
                emit q->showNotify(JCmdModel::tr("Answer arguments error! (Execute failure)"),
                                   + "<br/><i><font size=4>"
                                   + JCmdModel::tr("Arguments: ")
                                   + "</font></i>"
                                   + answerArgs);
            }
        }
        // callback
        QJSValueList args;
        // [0] - success
        args.append(success);
        // [1] - count
        args.append(count);
        // [2] - args
        args.append(answerArgs);
        // [3] - answer
        args.append(answer);
        // [4] - message
        if (!message.isEmpty()) {
            args.append(message);
        }
        QCoreApplication::postEvent(q, new JCallbackEvent(_callback, args));
    };

    if (currentCommand->isSync()) {
        QObject::connect(&watcherSync, &QFutureWatcher<int>::finished, q, [=](){
            reduce();
        });
        QObject::connect(&watcherSync, &QFutureWatcher<int>::canceled, q, [=](){
            reduce();
        });
        watcherSync.setFuture(futureSync);
    } else {
        QObject::connect(&watcherAsync, &QFutureWatcher<QSharedPointer<JCmdChannel> >::resultReadyAt,
                         q, [=](int resultIndex){
            Q_UNUSED(resultIndex);
            auto channel = watcherAsync.resultAt(resultIndex);
            if (channel->workState() == JCmdChannel::WorkStateFinish) {
                channel->startIdleTimer();
            }
            reduce();
        });
        QObject::connect(&watcherAsync, &QFutureWatcher<QSharedPointer<JCmdChannel> >::finished,
                         q, [=](){
            reduce();
        });
        QObject::connect(&watcherAsync, &QFutureWatcher<QSharedPointer<JCmdChannel> >::canceled,
                         q, [=](){
            reduce();
        });
        watcherAsync.setFuture(futureAsync);
    }

    return true;
}

void JCmdModelPrivate::showNotifyError(const QString &command, QJSValue callback,
                                       JCmdModel::ExecuteError flag,
                                       const QSharedPointer<icdmeta::JCmdObject> &object)
{
    Q_Q(JCmdModel);
    // callback
    if (callback.isCallable()) {
        QJSValueList args;
        // [0] - success
        args.append(false);
        QCoreApplication::postEvent(q, new JCallbackEvent(callback, args));
    }

    const QString workModeName = q->workModeName(q->workMode());
    QString guid;

    // show notify
    QString message, detail;
    switch (flag) {
    case JCmdModel::SendNoValidChannel:
        message = JCmdModel::tr("Has no valid channel!");
        break;
    case JCmdModel::SendInvalidCmd:
        message = JCmdModel::tr("Command \"%1\" is not exists!").arg(command);
        detail = JCmdModel::tr("Current Mode: %1").arg(workModeName);
        break;
    case JCmdModel::SendInvalidParam:
        message = JCmdModel::tr("Parameter object is null!");
        break;
    case JCmdModel::SendFrequentlyFailure:
        guid = QUuid::createUuid().toString();
        message = JCmdModel::tr("Command \"%1\" be sent too frequently!").arg(command);
        timerFreq->setProperty("guid", guid);
        break;
    case JCmdModel::SendLowPriorityFailure:
        guid = QUuid::createUuid().toString();
        if (currentCommand) {
            if (object->priority() < currentCommand->priority()) {
                message = JCmdModel::tr("The priority of command \"%1\" is too lowly, and command \"%2\" is running!");
            } else {
                message = JCmdModel::tr("The priority of command \"%1\" is equal to command \"%2\" and cannot execute!");
            }
            message = message.arg(command).arg(currentCommand->desc());
            currentCommand->setCallback([=](const QString &guid){
                q->hideNotify(guid);
            }, guid);
        } else {
            message = JCmdModel::tr("The priority of command \"%1\" is too lowly!").arg(command);
        }
        detail = JCmdModel::tr("Current Mode: %1").arg(workModeName);
        break;
    case JCmdModel::SendSysBlockingFailure:
        guid = QUuid::createUuid().toString();
        if (currentCommand) {
            message = JCmdModel::tr("The command system is blocked!, and command \"%1\" is running!")
                    .arg(currentCommand->desc());
            currentCommand->setCallback([=](const QString &guid){
                q->hideNotify(guid);
            }, guid);
        } else {
            message = JCmdModel::tr("The command system is blocked!, and some command is active!");
        }
        detail = JCmdModel::tr("Current Mode: %1").arg(workModeName);
        break;
    case JCmdModel::SendCmdBlockingFailure:
        guid = QUuid::createUuid().toString();
        message = JCmdModel::tr("The command system is blocked, and command \"%1\" is running!");
        if (currentCommand) {
            message = message.arg(currentCommand->desc());
            currentCommand->setCallback([=](const QString &guid){
                q->hideNotify(guid);
            }, guid);
        }
        detail = JCmdModel::tr("Current Mode: %1").arg(workModeName);
        break;
    case JCmdModel::SendReadyFailure:
        message = JCmdModel::tr("Command \"%1\" was ready fo sending failure!").arg(command);
        break;
    case JCmdModel::ExecuteArgFailure:
        message = JCmdModel::tr("Command \"%1\" was executed failure!").arg(command);
        if (object) {
            //
        }
        break;
    default:
        Q_ASSERT(false);
        return;
    }

    emit q->showNotify(message, detail, guid);
}

void JCmdModelPrivate::finishWatcher(bool flush)
{
    Q_Q(JCmdModel);

    for (auto &channel : channels) {
        channel->cancel();
    }

    //
    if (watcherSync.isRunning()) {
        watcherSync.cancel();
        watcherSync.waitForFinished();
        if (flush) {
            emit watcherSync.canceled();
        }
        watcherSync.disconnect(q);
    }

    //
    if (watcherAsync.isRunning()) {
        watcherAsync.cancel();
        watcherAsync.waitForFinished();
        if (flush) {
            emit watcherAsync.canceled();
        }
        watcherAsync.disconnect(q);
    }

    //
    if (!callback.isUndefined()) {
        QJSValueList args;
        // success
        args.append(false);
        // count
        args.append(0);
        callback.call(args);
    }
    callback = QJSValue();
    if (currentCommand) {
        currentCommand->callWithClear();
        currentCommand = nullptr;
    }
}

void JCmdModelPrivate::reset()
{
    finishWatcher(true);
    workMode = JCmdChannel::WorkModeHome;
}

template<typename T>
void JCmdModelPrivate::replaceValue(Json::Value &value, const std::string &key,
                                    const T &defaultValue)
{
    if (!value.isObject()) {
        return;
    }

    if (value.isMember(key)) {
        const Json::Value _value = value[key];
        if (_value.isString()) {
            const std::string sValue = _value.asString();
            if (sValue.empty() || sValue == "default") {
                value[key] = defaultValue;
            }
        }
    } else {
        value[key] = defaultValue;
    }
}

// class JCmdModel

J_QML_IMPLEMENT_SINGLE_INSTANCE(JCmdModel, QQmlEngine::CppOwnership, IcdCore)

JCmdModel::JCmdModel(QObject *parent)
    : QObject(parent)
    , J_DPTR(new JCmdModelPrivate(this))
{
    Q_D(JCmdModel);
    d->init();
}

JCmdModel::~JCmdModel()
{
    Q_D(JCmdModel);
    close();
    delete d;
}

void JCmdModel::registerQmlType()
{
    //
    IcdMetaRegisterSingletonType3(JCmdModel);

    //
    JCmdChannel::registerQmlType();
    JCmdSysModelSim::registerQmlType();
}

int JCmdModel::workMode() const
{
    Q_D(const JCmdModel);
    return d->workMode;
}

QQmlListProperty<icdmeta::JCmdChannel> JCmdModel::channels()
{
    return QQmlListProperty<icdmeta::JCmdChannel>(this, 0,
                                                  &JCmdModelPrivate::channelsCount,
                                                  &JCmdModelPrivate::channelsAt);
}

bool JCmdModel::blocking() const
{
    Q_D(const JCmdModel);
    return d->blocking;
}

bool JCmdModel::isFinished() const
{
    Q_D(const JCmdModel);
    return !d->blocking && d->watcherAsync.isFinished() && d->watcherSync.isFinished();
}

icdmeta::JCmdObject *JCmdModel::currentCommand() const
{
    Q_D(const JCmdModel);
    return d->currentCommand.data();
}

icdmeta::JCmdChannel *JCmdModel::findItem(const QString &identity) const
{
    Q_D(const JCmdModel);
    QListIterator<QSharedPointer<JCmdChannel> > citer(d->channels);
    while (citer.hasNext()) {
        const QSharedPointer<JCmdChannel> &channel = citer.next();
        if (channel->identity() == identity) {
            return channel.data();
        }
    }

    return Q_NULLPTR;
}

bool JCmdModel::send(const QString &command, QJSValue callback, bool force)
{
    Q_D(JCmdModel);
    auto object = d->findCommand(command, d->workMode);
    if (!object) {
        return false;
    }
    return d->send(object, callback, force);
}

bool JCmdModel::send(const QString &command, const QString &arguments,
                     QJSValue callback, bool force)
{
    Q_D(JCmdModel);
    auto object = d->findCommand(command, d->workMode);
    if (!object) {
        return false;
    }
    object->setArguments(arguments);
    return d->send(object, callback, force);
}

bool JCmdModel::send(const QString &command, bool sync, QJSValue callback, bool force)
{
    Q_D(JCmdModel);
    auto object = d->findCommand(command, d->workMode);
    if (!object) {
        return false;
    }
    object->setSync(sync);
    return d->send(object, callback, force);
}

bool JCmdModel::send(const QString &command, const QString &arguments,
                     bool sync, QJSValue callback, bool force)
{
    Q_D(JCmdModel);
    auto object = d->findCommand(command, d->workMode);
    if (!object) {
        return false;
    }
    object->setArguments(arguments);
    object->setSync(sync);
    return d->send(object, callback, force);
}

bool JCmdModel::send(const QString &command, int timeout, QJSValue callback,
                     bool force)
{
    Q_D(JCmdModel);
    auto object = d->findCommand(command, d->workMode);
    if (!object) {
        return false;
    }
    object->setTimeout(timeout);
    return d->send(object, callback, force);
}

bool JCmdModel::send(const QString &command, const QString &arguments,
                     int timeout, QJSValue callback, bool force)
{
    Q_D(JCmdModel);
    auto object = d->findCommand(command, d->workMode);
    if (!object) {
        return false;
    }
    object->setArguments(arguments);
    object->setTimeout(timeout);
    return d->send(object, callback, force);
}

bool JCmdModel::send(const QString &command, int timeout, bool sync,
                     QJSValue callback, bool force)
{
    Q_D(JCmdModel);
    auto object = d->findCommand(command, d->workMode);
    if (!object) {
        return false;
    }
    object->setSync(sync);
    object->setTimeout(timeout);
    return d->send(object, callback, force);
}

bool JCmdModel::send(const QString &command, const QString &arguments,
                     int timeout, bool sync, QJSValue callback, bool force)
{
    Q_D(JCmdModel);
    auto object = d->findCommand(command, d->workMode);
    if (!object) {
        return false;
    }
    object->setArguments(arguments);
    object->setSync(sync);
    object->setTimeout(timeout);
    return d->send(object, callback, force);
}

QString JCmdModel::workModeName(int value) const
{
    return JCmdChannel::workModeName(value);
}

QString JCmdModel::workModeNameFromCommand(const QString &command)
{
    return JCmdChannel::workModeNameFromCommand(command);
}

QString JCmdModel::workModeNameFromProtocol(int value)
{
    return JCmdChannel::workModeNameFromProtocol(value);
}

QString JCmdModel::workModeString(int value) const
{
    return JCmdChannel::workModeString(value);
}

int JCmdModel::stringWorkMode(const QString &value, bool contains) const
{
    return JCmdChannel::stringWorkMode(value, contains);
}

bool JCmdModel::setCurrentCommandCallback(QJSValue callback, const QString &guid)
{
    Q_D(JCmdModel);
    if (d->currentCommand) {
        d->currentCommand->setCallback(callback, guid);
        return true;
    } else {
        return true;
    }
}

void JCmdModel::clearCurrentCommandCallback(const QString &guid)
{
    Q_D(JCmdModel);
    if (d->currentCommand) {
        d->currentCommand->clearCallback(guid);
    }
}

void JCmdModel::setWorkMode(int value)
{
    Q_D(JCmdModel);
    if (value != d->workMode) {
        d->workMode = value;
        for (auto &channel : d->channels) {
            channel->setWorkMode(value);
        }
        emit workModeChanged(value);
    }
}

void JCmdModel::setBlocking(bool value)
{
    Q_D(JCmdModel);
    if (value != d->blocking) {
        d->blocking = value;
        emit blockingChanged(value);
    }
}

void JCmdModel::appendChannel(icdmeta::JChannel *channel)
{
    Q_D(JCmdModel);
    JCmdChannel *newChannel = new JCmdChannel(channel->identity(), this);
    newChannel->binding(channel);
    d->channels.append(QSharedPointer<JCmdChannel>(newChannel, &QObject::deleteLater));
    emit channelsChanged();
}

void JCmdModel::clearChannel()
{
    Q_D(JCmdModel);
    close();
    if (!d->channels.isEmpty()) {
        d->channels.clear();
        emit channelsChanged();
    }
}

bool JCmdModel::open()
{
    Q_D(JCmdModel);
    if (d->channels.isEmpty()) {
        return false;
    }

    for (auto &channel : d->channels) {
        channel->reset();
    }

    finishWatcher();

    for (auto &channel : d->channels) {
        if (!channel->open()) {
            return false;
        }
        channel->reset();
    }

    return true;
}

void JCmdModel::close()
{
    Q_D(JCmdModel);
    d->reset();
    for (auto &channel : d->channels) {
        channel->reset();
    }

    for (auto &channel : d->channels) {
        channel->close();
    }
}

void JCmdModel::finishWatcher()
{
    Q_D(JCmdModel);
    if (d->timerFreq->isActive()) {
        d->timerFreq->stop();
        QCoreApplication::removePostedEvents(this, QEvent::Timer);
    }
    d->finishWatcher(true);
    emit finishWatcherNotify();
}

void JCmdModel::reset()
{
    clearChannel();
}

void JCmdModel::customEvent(QEvent *event)
{
    switch (event->type()) {
    case Event_Callback:
    {
        JCallbackEvent *cbEvent = reinterpret_cast<JCallbackEvent *>(event);
        if (!cbEvent) {
            break;
        }
        cbEvent->accept();
        cbEvent->execute();
        break;
    }
    default:
        break;
    }
}

}
