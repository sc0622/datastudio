#include "precomp.h"
#include "JCmdObject.h"

namespace icdmeta {

// class JCmdObjectPrivate

class JCmdObjectPrivate
{
public:
    JCmdObjectPrivate(JCmdObject *q)
        : J_QPTR(q)
        , sync(false)
        , timeout(2000)     // unit: ms
        , blocking(false)
        , priority(0)
        , acceptCount(1)
        , callback(nullptr)
    {

    }

    void init();

    JCmdObjectPrivate &operator =(const JCmdObjectPrivate &rhs);

private:
    J_DECLARE_PUBLIC(JCmdObject)
    QString name;
    QString suffix;
    QString pattern;
    bool sync;
    int timeout;
    QString message;
    QString desc;
    bool blocking;
    int priority;
    int acceptCount;
    QString arguments;
    QString parsedPattern;
    std::function<void(QString)> callback;
    QString guid;
    QJSValue jsCallback;
    QString jsGuid;
};

void JCmdObjectPrivate::init()
{

}

JCmdObjectPrivate &JCmdObjectPrivate::operator =(const JCmdObjectPrivate &rhs)
{
    if (this == &rhs) {
        return *this;
    }
    name = rhs.name;
    arguments = rhs.arguments;
    suffix = rhs.suffix;
    pattern = rhs.pattern;
    parsedPattern = rhs.parsedPattern;
    sync = rhs.sync;
    timeout = rhs.timeout;
    message = rhs.message;
    desc = rhs.desc;
    blocking = rhs.blocking;
    priority = rhs.priority;
    acceptCount = rhs.acceptCount;
    callback = rhs.callback;
    guid = rhs.guid;
    jsCallback = rhs.jsCallback;
    jsGuid = rhs.jsGuid;
    return *this;
}

// class JCmdObject

JCmdObject::JCmdObject(QObject *parent)
    : QObject(parent)
    , J_DPTR(new JCmdObjectPrivate(this))
{
    Q_D(JCmdObject);
    d->init();
}

JCmdObject::~JCmdObject()
{
    Q_D(JCmdObject);
    delete d;
}

void JCmdObject::registerQmlType()
{
    //
    IcdMetaRegisterUncreatableType2(JCmdObject);

    //
}

bool JCmdObject::isValid() const
{
    Q_D(const JCmdObject);
    if (d->name.isEmpty()) {
        return false;
    }
    return true;
}

QString JCmdObject::name() const
{
    Q_D(const JCmdObject);
    return d->name;
}

QString JCmdObject::arguments() const
{
    Q_D(const JCmdObject);
    return d->arguments;
}

QString JCmdObject::suffix() const
{
    Q_D(const JCmdObject);
    return d->suffix;
}

QString JCmdObject::pattern() const
{
    Q_D(const JCmdObject);
    return d->pattern;
}

QString JCmdObject::parsedPattern() const
{
    Q_D(const JCmdObject);
    return d->parsedPattern;
}

bool JCmdObject::isSync() const
{
    Q_D(const JCmdObject);
    return d->sync;
}

int JCmdObject::timeout() const
{
    Q_D(const JCmdObject);
    return d->timeout;
}

QString JCmdObject::message() const
{
    Q_D(const JCmdObject);
    return d->message;
}

QString JCmdObject::desc() const
{
    Q_D(const JCmdObject);
    return d->desc;
}

bool JCmdObject::blocking() const
{
    Q_D(const JCmdObject);
    return d->blocking;
}

int JCmdObject::priority() const
{
    Q_D(const JCmdObject);
    return d->priority;
}

int JCmdObject::acceptCount() const
{
    Q_D(const JCmdObject);
    return d->acceptCount;
}

JCmdObject *JCmdObject::clone() const
{
    return new JCmdObject(*this);
}

JCmdObject &JCmdObject::operator =(const JCmdObject &other)
{
    Q_D(JCmdObject);
    *d = *other.J_DPTR;
    return *this;
}

QByteArray JCmdObject::toByteArray() const
{
    Q_D(const JCmdObject);
    QByteArray data;
    // command
    data.append(d->name);
    // arguments
    if (!d->arguments.isEmpty()) {
        data.append(' ').append(d->arguments);
    }
    // suffix
    data.append(" ");   //TODO
    data.append(d->suffix);

    return data;
}

void JCmdObject::setCallback(const std::function<void(QString)> &callback,
                            const QString &guid)
{
    Q_D(JCmdObject);
    d->callback = callback;
    d->guid = guid;
}

void JCmdObject::setCallback(QJSValue &callback, const QString &guid)
{
    Q_D(JCmdObject);
    d->jsCallback = callback;
    d->jsGuid = guid;
}

bool JCmdObject::callWithClear()
{
    Q_D(JCmdObject);
    if (d->callback != Q_NULLPTR) {
        d->callback(d->guid);
        d->callback = Q_NULLPTR;
        d->guid = QString();
        return true;
    }

    if (d->jsCallback.isCallable()) {
        QJSValueList args;
        args.append(d->jsGuid);
        d->jsCallback.call(args);
        d->jsCallback = QJSValue();
        d->jsGuid = QString();
        return true;
    }

    return false;
}

void JCmdObject::clearCallback(const QString &guid)
{
    Q_D(JCmdObject);
    if (d->guid == guid) {
        d->callback = Q_NULLPTR;
        d->guid = QString();
    }
    if (d->jsGuid == guid) {
        d->jsCallback = QJSValue();
        d->jsGuid = QString();
    }
}

JCmdObject::JCmdObject(const JCmdObject &other)
    : J_DPTR(new JCmdObjectPrivate(this))
{
    operator =(other);
}

void JCmdObject::setArguments(const QString &value)
{
    Q_D(JCmdObject);
    if (value != d->arguments) {
        d->arguments = value;
        emit argumentsChanged(value);
    }
}

void JCmdObject::setName(const QString &value)
{
    Q_D(JCmdObject);
    if (value != d->name) {
        d->name = value;
        emit nameChanged(value);
    }
}

void JCmdObject::setSuffix(const QString &value)
{
    Q_D(JCmdObject);
    if (value != d->suffix) {
        d->suffix = value;
        emit suffixChanged(value);
    }
}

void JCmdObject::setPattern(const QString &value)
{
    Q_D(JCmdObject);
    if (value != d->pattern) {
        d->pattern = value;
        emit patternChanged(value);
    }
}

void JCmdObject::setSync(bool value)
{
    Q_D(JCmdObject);
    if (value != d->sync) {
        d->sync = value;
        emit syncChanged(value);
    }
}

void JCmdObject::setTimeout(int value)
{
    Q_D(JCmdObject);
    if (value != d->timeout) {
        d->timeout = value;
        emit timeoutChanged(value);
    }
}

void JCmdObject::setMessage(const QString &value)
{
    Q_D(JCmdObject);
    if (value != d->message) {
        d->message = value;
        emit messageChanged(value);
    }
}

void JCmdObject::setDesc(const QString &value)
{
    Q_D(JCmdObject);
    if (value != d->desc) {
        d->desc = value;
        emit descChanged(value);
    }
}

void JCmdObject::setBlocking(bool value)
{
    Q_D(JCmdObject);
    if (value != d->blocking) {
        d->blocking = value;
        emit blockingChanged(value);
    }
}

void JCmdObject::setPriority(int value)
{
    Q_D(JCmdObject);
    if (value != d->priority) {
        d->priority = value;
        emit priorityChanged(value);
    }
}

void JCmdObject::setAcceptCount(int value)
{
    Q_D(JCmdObject);
    if (value != d->acceptCount) {
        d->acceptCount = value;
        emit acceptCountChanged(value);
    }
}

Json::Value JCmdObject::save() const
{
    Q_D(const JCmdObject);
    Json::Value json;
    json["name"] = d->name.toStdString();
    json["pattern"] = d->pattern.toStdString();
    json["sync"] = d->sync;
    json["timeout"] = d->timeout;
    json["message"] = d->message.toStdString();
    json["desc"] = d->desc.toStdString();
    json["blocking"] = d->blocking;
    json["priority"] = d->priority;
    json["acceptCount"] = d->acceptCount;
    return json;
}

bool JCmdObject::restore(const Json::Value &json, int)
{
    setName(QString::fromStdString(json["name"].asString()));
    setPattern(QString::fromStdString(json["pattern"].asString()));
    setSync(json["sync"].asBool());
    setTimeout(json["timeout"].asInt());
    setMessage(QString::fromStdString(json["message"].asString()));
    setDesc(QString::fromStdString(json["desc"].asString()));
    setBlocking(json["blocking"].asBool());
    setPriority(json["priority"].asInt());
    setAcceptCount(json["acceptCount"].asInt());

    return true;
}

void JCmdObject::parsePattern()
{
    Q_D(JCmdObject);
    if (d->pattern.isEmpty()) {
        return;
    }
    d->parsedPattern = d->pattern;
    // replace parameters "%i" in pattern
    QStringList arguments = d->arguments.split(' ', QString::SkipEmptyParts);
    for (int i = 0; i < arguments.count(); ++i) {
        d->parsedPattern.replace("%" + QString::number(i + 1), arguments[i].trimmed());
    }
    // replace regexp in pattern
    d->parsedPattern.replace("%bool%", "((true)|(false))");
    d->parsedPattern.replace("%BOOL%", "((TRUE)|(FALSE))");
    d->parsedPattern.replace("%Switch%", "((On)|(Off))");
}

}
