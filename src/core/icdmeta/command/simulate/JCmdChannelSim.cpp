#include "precomp.h"
#include "JCmdChannelSim.h"
#include "../../channel/JChannel.h"
#include "../../channel/JSuperChannel.h"

namespace icdmeta {

struct AnswerData
{
    QString command;
    QString extra;
    QString suffix;
    int interval;
    QTimer *timer;

    AnswerData() : interval(0), timer(nullptr) {}
    AnswerData(const QString &command) : command(command), timer(nullptr)  {}
    AnswerData(const QString &command, const QString &extra,
               const QString &suffix, int interval)
        : command(command), extra(extra), suffix(suffix), interval(interval), timer(nullptr)
    {}

    ~AnswerData()
    {
        if (timer) {
            timer->disconnect(timer->parent());
            QApplication::removePostedEvents(timer->parent(), QEvent::Timer);
            timer->deleteLater();
        }
    }

    AnswerData(const AnswerData &rhs)
    { operator =(rhs); }

    AnswerData &operator =(const AnswerData &rhs)
    {
        if (this == &rhs) {
            return *this;
        }
        command = rhs.command;
        extra = rhs.extra;
        suffix = rhs.suffix;
        interval = rhs.interval;
        timer = rhs.timer;
        return *this;
    }

    bool operator ==(const AnswerData &rhs)
    {
        return (command == rhs.command);
    }
};

// class JCmdChannelSimPrivate

class JCmdChannelSimPrivate
{
public:
    JCmdChannelSimPrivate(JCmdChannelSim *q)
        : J_QPTR(q)
        , channel(Q_NULLPTR)
        , nativeChannel(Q_NULLPTR)
        , workMode(JCmdChannel::WorkModeHome)
        , workState(JCmdChannel::WorkStateIdle)
        , currentWorkMode(JCmdChannel::WorkModeHome)
        , suffix("\n")
    {

    }

    void init();
    bool loadConfig();
    void timeout();
    void cancel();
    void reset();
    void clear();
    void readySending(const QString &data, const QString &extra = QString(),
                      const QString &suffix = QString());
    bool send(const QByteArray &data);
    bool receive();
    bool process(const QString &_answer);

    bool startTimeout(int msecs);
    void stopTimeout();

    void removeAnswerData(const QString &command);

private:
    J_DECLARE_PUBLIC(JCmdChannelSim)
    QMutex mutex;
    QString identity;
    icdmeta::JChannel *channel;
    icdmeta::JSuperChannel *nativeChannel;
    int workMode;
    int workState;
    int currentWorkMode;
    QTimer *timerTimeout;
    QString suffix;
    QByteArray command;
    QString answer;
    QQueue<AnswerData> answerQueue;
    QQueue<std::shared_ptr<AnswerData> > answerPost;
    QTimer *timerAnswerPost;
};

void JCmdChannelSimPrivate::init()
{
    Q_Q(JCmdChannelSim);
    timerTimeout = new QTimer(q);
    timerTimeout->setSingleShot(true);
    timerTimeout->setTimerType(Qt::PreciseTimer);
    QObject::connect(timerTimeout, &QTimer::timeout, q, [=](){
        timeout();
    });
    //
    timerAnswerPost = new QTimer(q);
    timerAnswerPost->setInterval(20);
    timerAnswerPost->setTimerType(Qt::PreciseTimer);
    QObject::connect(timerAnswerPost, &QTimer::timeout, q, [=](){
        if (answerQueue.isEmpty()) {
            return;
        }
        AnswerData *item = new AnswerData(answerQueue.dequeue());
        answerPost.enqueue(std::shared_ptr<AnswerData>(item));
        item->timer = new QTimer(q);
        item->timer->setInterval(item->interval);
        item->timer->setSingleShot(true);
        item->timer->setTimerType(Qt::PreciseTimer);
        QObject::connect(item->timer, &QTimer::timeout, q, [=](){
            readySending(item->command, item->extra, item->suffix);
            removeAnswerData(item->command);
        });
        item->timer->start();
    });
    timerAnswerPost->start();
    //
    if (!loadConfig()) {
        //
    }
}

bool JCmdChannelSimPrivate::loadConfig()
{
    QSettings settings;
    settings.beginGroup(QCoreApplication::applicationVersion().append("/Temp/Paths"));
    const QString filePath = settings.value("CONFIG_FILE").toString();
    settings.endGroup();

    const Json::Value commandJson = Json::resolve(filePath.toLocal8Bit(), "command");
    if (commandJson.isNull()) {
        return false;
    }

    if (commandJson.isNull()) {
        return false;
    }

    // suffix
    std::string _suffix = commandJson["suffix"].asString();
    Icd::replaceString(_suffix, "<br/>", "\r\n");
    suffix = QString::fromStdString(_suffix);

    return true;
}

void JCmdChannelSimPrivate::timeout()
{
    mutex.lock();
    workState = JCmdChannel::WorkStateRecv;
    mutex.unlock();
}

void JCmdChannelSimPrivate::cancel()
{
    mutex.lock();
    answer.clear();
    workState = JCmdChannel::WorkStateFinish;
    mutex.unlock();
}

void JCmdChannelSimPrivate::reset()
{
    mutex.lock();
    workState = JCmdChannel::WorkStateIdle;
    answer.clear();
    mutex.unlock();
    timerTimeout->stop();
}

void JCmdChannelSimPrivate::clear()
{
    mutex.lock();
    answer.clear();
    workState = JCmdChannel::WorkStateRecv;
    mutex.unlock();
}

void JCmdChannelSimPrivate::readySending(const QString &data, const QString &extra,
                                         const QString &suffix)
{
    mutex.lock();
    answer.clear();
    command.clear();
    // channel name
    command.append(nativeChannel->mark());
    if (data.endsWith(">")) {
        // workMode
        command.append(data);
    } else {
        // workMode
        command.append(JCmdChannel::workModeString(workMode))
                // command
                .append("Ans_")
                .append(data);
    }
    // extra
    if (!extra.isEmpty()) {
        command.append(extra);
    }
    //
    command.append('\r');
    // suffix
    if (suffix.isEmpty()) {
        command.append(this->suffix);
    } else {
        command.append(suffix);
    }
    workState = JCmdChannel::WorkStateSend;
    mutex.unlock();
}

bool JCmdChannelSimPrivate::send(const QByteArray &data)
{
    if (channel) {
        if (channel->writeData(data) == data.count()) {
            return true;
        }
    }
    return true;
}

bool JCmdChannelSimPrivate::receive()
{
    const QByteArray data = channel->readData(4096);
    if (data.isEmpty()) {
        return false;
    }

    //TEST (sync = false)
    if (nativeChannel->mark() == "C") {
        //return false;
    }

    mutex.lock();
    answer.append(data);
    QStringList answers = answer.section(suffix, 0, -2).split(suffix);
    answer = answer.section(suffix, -1);
    mutex.unlock();

    for (const auto &answer : answers) {
        if (answer.isEmpty()) {
            continue;
        }
        process(answer + suffix);
    }

    return true;
}

bool JCmdChannelSimPrivate::process(const QString &_answer)
{
    if (!_answer.endsWith(suffix)) {
        return false;
    }

    QString answer = _answer.section(suffix, 0, 0, QString::SectionSkipEmpty).trimmed();
    const QString command = answer.section(' ', 0, 0, QString::SectionSkipEmpty).trimmed();
    const QString args = answer.section(' ', 1, -1, QString::SectionSkipEmpty).trimmed();

    //
    if (command == "CPos") {
        if (workMode == JCmdChannel::WorkModeHome) {
#if 1
            readySending("\\>",
                         "\r\rVX_" + nativeChannel->mark() + ": TB10000\r\r"
                         + "CPU_" + nativeChannel->mark() + ": TB1_***0001\r\r"
                         + "MIO_A: TB1_***0002\r\r"
                         + "MIO_B: TB1_***0003\r\r");
#else   //TEST
            readySending("\\MIT>");
#endif
        } else {
#if 1
            readySending(JCmdChannel::workModeString(workMode));
#elif 0  //TEST
            readySending(CmdChannel::workModeString(currentWorkMode));
#else   //TEST
            readySending("\\>",
                         "\r\rVX_" + nativeChannel->mark() + ": TB10000\r\r"
                         + "CPU_" + nativeChannel->mark() + ": TB1_***0001\r\r"
                         + "MIO_A: TB1_***0002\r\r"
                         + "MIO_B: TB1_***0003\r\r");
#endif
        }
    } else if (QRegExp("((MIT)|(ROU)|(FLY))").exactMatch(command)) {
        readySending("\\" + command + ">");
    } else {
        // ---- global----
        if (command == "Load_ac_num") {
            readySending(command, "#TRUE");
            return true;
        }
        // ---- mit ----
        // ---- sensor ----
        else if (command == "Sensorload") {
            const QString module = args.section(' ', 2, 2);
            if (module == "FINSB") {
#if 0
                readySending(command, "#" + module + "_FALSE");
                return true;
#else
                readySending(command, "#"
                             + args.section(' ', 2, 2)
                             + "_TRUE");
#endif
            } else {
                readySending(command, "#"
                             + args.section(' ', 2, 2)
                             + "_TRUE");
            }
            return true;
        } else if (command == "RA_Power") {
            readySending(command, "#"
                         + args.section(' ', 0, 0));
            return true;
        } else if (command == "DBM") {
            readySending(command, "#"
                         + args.section(' ', 0, 0));
            return true;
        } else if (command == "RA_Revise") {
            readySending(command, "#TRUE");
            return true;
        }
        // ---- Rudder System ----
        else if (command == "ACT_MoveTo") {
            readySending(command, "#"
                         + args.section(' ', 0, 0)
                         + "_TRUE");
            return true;
        } else if (command == "Throttle_pos") {
            readySending(command, "#"
                         + args.section(' ', 0, 0)
                         + "_TRUE");
            return true;
        } else if (command == "ACT_TurnTo") {
            readySending(command, "#"
                         + args.section(' ', 0, 0)
                         + "_TRUE");
            return true;
        } else if (command == "TA_TurnTo") {
            readySending(command, "#"
                         + args.section(' ', 0, 0)
                         + "_TRUE");
            return true;
        } else if (command == "Cut_Ach") {
            readySending(command, "#"
                         + args.section(' ', 0, 0)
                         + "_" + args.section(' ', 1, 1)
                         + "_TRUE");
            return true;
        } else if (command == "NWS_cal") {
            readySending(command, "#"
                         + args.section(' ', 0, 0)
                         + "_TRUE");
            return true;
        } else if (command == "TAload_A") {
            readySending(command, "#"
                         + args.section(' ', 2, 2)
                         + "_" + args.section(' ', 3, 3)
                         + "_TRUE");
            return true;
        } else if (command == "TAload_B") {
            readySending(command, "#"
                         + args.section(' ', 2, 2)
                         + "_" + args.section(' ', 3, 3)
                         + "_TRUE");
            return true;
        } else if (command == "NWS_Revise") {
            readySending(command, "#TRUE");
            return true;
        } else if (command == "NWS_MoveTo") {
            readySending(command, "#TRUE");
            return true;
        } else if (command == "NWS_TurnTo") {
            readySending(command, "#TRUE");
            return true;
        }
        // ---- Crosslink System ----
        // ---- landing ----
        else if (command == "Brake") {
            readySending(command, "#"
                         + args.section(' ', 0, 0)
                         + "_" + args.section(' ', 1, 1));
            return true;
        }
        // TA
        else if (command == "Brake_power") {
            readySending(command, "#"
                         + args.section(' ', 0, 0));
            return true;
        } else if (command == "LGH_Up") {
            clear();
            //readySending(command, "#TRUE");
            return true;
        } else if (command == "LGH_Down") {
            answerQueue.enqueue({
                                    command, "#TRUE", "", 1000
                                });
            return true;
        } else if (command == "LGH_Up_cancel") {
            removeAnswerData("LGH_Up");
            readySending(command, "#TRUE");
            return true;
        } else if (command == "LGH_Down_cancel") {
            removeAnswerData("LGH_Down");
            readySending(command, "#TRUE");
            return true;
        }
        // ---- engine ----
        else if (command == "Engine_power") {
            readySending(command, "#"
                         + args.section(' ', 0, 0)
                         + "_" + args.section(' ', 1, 1));
            return true;
        }
        // TA
        else if (command == "Engine_pump") {
            readySending(command, "#"
                         + args.section(' ', 0, 0)
                         + "_" + args.section(' ', 1, 1));
            return true;
        } else if (command == "Engine_fire_en") {
            readySending(command, "#"
                         + args.section(' ', 0, 0)
                         + "_" + args.section(' ', 1, 1)
                         + "_" + args.section(' ', 2, 2));
            return true;
        } else if (command == "Engine_fire_dis") {
            readySending(command, "#"
                         + args.section(' ', 0, 0)
                         + "_" + args.section(' ', 1, 1));
            return true;
        } else if (command == "Engine_start") {
            readySending(command, "#"
                         + args.section(' ', 0, 0)
                         + "_Off");
            return true;
        } else if (command == "Engine_rspeed") {
            readySending(command, "#"
                         + args.section(' ', 0, 0)
                         + "_" + args.section(' ', 1, 1));
            return true;
        } else if (command == "Engine_emergency") {
            readySending(command, "#"
                         + args.section(' ', 0, 0)
                         + "_" + args.section(' ', 1, 1));
            return true;
        } else if (command == "Fuel_Revise") {
            readySending(command, "#TRUE");
            return true;
        }
        // ---- power ----
        //TA
        else if (command == "Generator_enable") {
            readySending(command, "#"
                         + args.section(' ', 0, 0)
                         + "_" + args.section(' ', 1, 1));
            return true;
        } else if (command == "Generator_reset") {
            readySending(command, "#"
                         + args.section(' ', 0, 0)
                         + "_" + args.section(' ', 1, 1));
            return true;
        } else if (command == "Generator_dlc") {
            readySending(command, "#"
                         + args.section(' ', 0, 0)
                         + "_" + args.section(' ', 1, 1));
            return true;
        } else if (command == "Generator_epc") {
            readySending(command, "#"
                         + args.section(' ', 0, 0)
                         + "_" + args.section(' ', 1, 1));
            return true;
        } else if (command == "Freeze_power") {
            readySending(command, "#"
                         + args.section(' ', 0, 0));
            return true;
        }
        // TB
        else if (command == "Generator_emergency") {
            readySending(command, "#"
                         + args.section(' ', 0, 0)
                         + "_" + args.section(' ', 1, 1));
            return true;
        }
        // ---- electrify ----
        else if (command == "Taskcomputer") {
            readySending(command, "#"
                         + args.section(' ', 0, 0));
            return true;
        } else if (command == "Pitot_heat_on") {
            answerQueue.enqueue({
                                    command, "#Off", "",
                                    int(args.section(' ', 0, 0).toDouble() * 1000)
                                });
            return true;
        } else if (command == "Pitot_heat_off") {
            removeAnswerData("Pitot_heat_on");
            readySending(command, "#Off");
            return true;
        } else if (command == "Identification") {
            readySending(command, "#"
                         + args.section(' ', 0, 0));
            return true;
        } else if (command == "DBM") {
            readySending(command, "#"
                         + args.section(' ', 0, 0));
            return true;
        } else if (command == "Light") {
            readySending(command, "#"
                         + args.section(' ', 0, 0));
            return true;
        } else if (command == "RA_power") {
            readySending(command, "#"
                         + args.section(' ', 0, 0));
            return true;
        } else if (command == "Satellite_com") {
            readySending(command, "#"
                         + args.section(' ', 0, 0));
            return true;
        } else if (command == "Hit_light") {
            readySending(command, "#"
                         + args.section(' ', 0, 0));
            return true;
        }
        // ---- taskpc ----
        else if (command == "Taskcomputer") {
            readySending(command, "#"
                         + args.section(' ', 0, 0));
            return true;
        }
        // ---- iff ----
        else if (command == "Emerback_en") {
            readySending(command, "#"
                         + args.section(' ', 0, 0));
            return true;
        }
        // ---- others ----
        //qDebug() << "----:" << answer;
        readySending(command.section(' ', 0, 0));
    }

    return true;
}

bool JCmdChannelSimPrivate::startTimeout(int msecs)
{
    return QMetaObject::invokeMethod(timerTimeout, "start", Qt::AutoConnection,
                                     Q_ARG(int, msecs));
}

void JCmdChannelSimPrivate::stopTimeout()
{
    QMetaObject::invokeMethod(timerTimeout, "stop", Qt::AutoConnection);
}

void JCmdChannelSimPrivate::removeAnswerData(const QString &command)
{
    QMutableListIterator<std::shared_ptr<AnswerData> > iter(answerPost);
    while (iter.hasNext()) {
        auto item = iter.next();
        if (item->command == command) {
            iter.remove();
        }
    }
}

// class JCmdChannelSim

JCmdChannelSim::JCmdChannelSim(const QString &identity, QObject *parent)
    : QObject(parent)
    , J_DPTR(new JCmdChannelSimPrivate(this))
{
    Q_D(JCmdChannelSim);
    d->identity = identity;
    d->init();
}

JCmdChannelSim::~JCmdChannelSim()
{
    Q_D(JCmdChannelSim);
    reset();
    delete d;
}

void JCmdChannelSim::registerQmlType()
{
    //
    IcdMetaRegisterUncreatableType2(JCmdChannelSim);

    //
}

QString JCmdChannelSim::identity() const
{
    Q_D(const JCmdChannelSim);
    return d->identity;
}

icdmeta::JChannel *JCmdChannelSim::channel() const
{
    Q_D(const JCmdChannelSim);
    return d->channel;
}

icdmeta::JSuperChannel *JCmdChannelSim::nativeChannel() const
{
    Q_D(const JCmdChannelSim);
    return d->nativeChannel;
}

int JCmdChannelSim::workMode() const
{
    Q_D(const JCmdChannelSim);
    return d->workMode;
}

int JCmdChannelSim::workState() const
{
    Q_D(const JCmdChannelSim);
    return d->workState;
}

int JCmdChannelSim::currentWorkMode() const
{
    Q_D(const JCmdChannelSim);
    return d->currentWorkMode;
}

void JCmdChannelSim::setCurrentWorkMode(int value)
{
    Q_D(JCmdChannelSim);
    if (value != d->currentWorkMode) {
        d->currentWorkMode = value;
        emit currentWorkModeChanged(value);
    }
}

void JCmdChannelSim::setWorkMode(int value)
{
    Q_D(JCmdChannelSim);
    if (value != d->workMode) {
        d->workMode = value;
        emit workModeChanged(value);
    }
}

bool JCmdChannelSim::binding(icdmeta::JChannel *channel)
{
    Q_D(JCmdChannelSim);
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

void JCmdChannelSim::unbinding()
{
    Q_D(JCmdChannelSim);
    if (d->channel) {
        d->cancel();
        d->mutex.lock();
        d->channel = Q_NULLPTR;
        d->nativeChannel = Q_NULLPTR;
        d->mutex.unlock();
        emit channelChanged();
        emit nativeChannelChanged();
    }
}

bool JCmdChannelSim::open()
{
    Q_D(JCmdChannelSim);
    if (!d->nativeChannel) {
        return false;
    }
    return d->nativeChannel->open();
}

void JCmdChannelSim::close()
{
    Q_D(JCmdChannelSim);
    if (!d->nativeChannel) {
        return;
    }
    d->nativeChannel->close();
}

void JCmdChannelSim::timeout()
{
    Q_D(JCmdChannelSim);
    d->timeout();
}

void JCmdChannelSim::cancel()
{
    Q_D(JCmdChannelSim);
    d->cancel();
    emit canceled();
}

void JCmdChannelSim::reset()
{
    Q_D(JCmdChannelSim);
    d->reset();
}

void JCmdChannelSim::run()
{
    Q_D(JCmdChannelSim);
    if (!d->channel) {
        return;
    }

    d->mutex.lock();
    d->workState = JCmdChannel::WorkStateRecv;
    d->mutex.unlock();

    int workState;

    while (true) {
        bool _break = false;
        d->mutex.lock();
        workState = d->workState;
        d->mutex.unlock();
        switch (workState) {
        case JCmdChannel::WorkStateSend:
        {
            d->mutex.lock();
            d->channel->writeData(d->command);
            d->command.clear();
            d->answer.clear();
            d->workState = JCmdChannel::WorkStateRecv;
            d->mutex.unlock();
            break;
        }
        case JCmdChannel::WorkStateRecv:
        {
            if (!d->receive()) {
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
        if (workState == JCmdChannel::WorkStateIdle
                || workState == JCmdChannel::WorkStateFinish) {
            break;
        }

        QThread::msleep(20);
    }

    d->stopTimeout();
}

}
