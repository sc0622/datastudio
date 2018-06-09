#include "precomp.h"
#include "JProtocol.h"
#include "../core/jicdtable.h"
#include "../common/JWatcher.h"
#include "icdparser/icdparser.h"
#include "icdcore/icd_item.h"

namespace icdmeta {

class FeedbackEvent : public QEvent
{
    Q_GADGET
public:
    explicit FeedbackEvent(JProtocol *sender, QJSValue callback,
                           bool success, JProtocol::LoadError error)
        : QEvent(static_cast<Type>(Event_ProtocolFeedback))
        , sender(sender)
        , callback(callback)
        , success(success)
        , error(error)
    {

    }

    void execute()
    {
        if (!callback.isCallable()) {
            return;
        }
        QJSValue _callback = callback;
        callback = QJSValue();
        QJSValueList arguments;
        arguments.append(success);
        arguments.append(error);
        QJSValue ret = _callback.call(arguments);
        if (ret.isError()) {
            qDebug() << ret.toString();
        }
        emit sender->loaded(success, error);
    }

private:
    JProtocol *sender;
    QJSValue callback;
    bool success;
    JProtocol::LoadError error;
};

// class JProtocolPrivate

class JProtocolPrivate
{
public:
    JProtocolPrivate(JProtocol *q)
        : J_QPTR(q)
        , table(nullptr)
    {

    }

    void init();
    bool load(QJSValue callback);
    void unload();
    void feedback(QJSValue callback, bool success,
                  JProtocol::LoadError error = JProtocol::LoadNoError);

private:
    J_DECLARE_PUBLIC(JProtocol)
    QString identity;
    QString domain;
    QString tableName;
    QString filePath;
    QString queryPath;
    QString channelId;
    QSharedPointer<icdmeta::JIcdTable> table;
    QQueue<QSharedPointer<JWatcher<Icd::TablePtr> > > watchers;
};

void JProtocolPrivate::init()
{

}

bool JProtocolPrivate::load(QJSValue callback)
{
    Q_Q(JProtocol);

    if (table) {
        feedback(callback, true);
        return true;
    }

    if (filePath.isEmpty() || queryPath.isEmpty()) {
        return false;
    }

    QSharedPointer<JWatcher<Icd::TablePtr> > newWatcher =
            QSharedPointer<JWatcher<Icd::TablePtr> >(new JWatcher<Icd::TablePtr>(callback, q),
                                                     jdelete_qobject);
    watchers.push_back(newWatcher);

    QFuture<Icd::TablePtr> future = QtConcurrent::run([=]() -> Icd::TablePtr {
        Icd::TablePtr table;
        Json::Value config;
        config["sourceType"] = "file";
        config["filePath"] = filePath.toLocal8Bit().data();
        Icd::ParserPtr parser = Icd::Parser::create(config);
        if (!parser->parse(queryPath.section('/', 0, 0).toStdString(),
                           queryPath.section('/', 1, 1).toStdString(),
                           queryPath.section('/', 2, 2).toStdString(),
                           table, Icd::ObjectItem)) {
            return nullptr;
        }
        return table;
    });
    auto deleteWatcher = [=](bool canceled){
        if (canceled) {
            newWatcher->cancel();
        }
        newWatcher->disconnect(q);
        watchers.removeOne(newWatcher);
    };
    auto reduce = [=](){
        const Icd::TablePtr _table = newWatcher->future().result();
        if (_table) {
            table = QSharedPointer<icdmeta::JIcdTable>(
                        new icdmeta::JIcdTable(_table, q), jdelete_qobject);
        }
        emit q->validChanged(table != nullptr);
        emit q->tableChanged(table.data());
        if (table) {
            feedback(newWatcher->callback(), true);
        } else {
            feedback(newWatcher->callback(), false, JProtocol::LoadParseError);
        }
    };
    QObject::connect(newWatcher.data(), &QFutureWatcher<Icd::TablePtr>::finished, q, [=](){
        reduce();
        deleteWatcher(false);
    });
    QObject::connect(newWatcher.data(), &QFutureWatcher<Icd::TablePtr>::canceled, q, [=](){
        deleteWatcher(true);
    });
    newWatcher->setFuture(future);

    return true;
}

void JProtocolPrivate::unload()
{
    Q_Q(JProtocol);
    table = nullptr;
    emit q->tableChanged(nullptr);
    emit q->validChanged(false);
    emit q->checkedChanged(false);
}

void JProtocolPrivate::feedback(QJSValue callback, bool success,
                                JProtocol::LoadError error)
{
    Q_Q(JProtocol);
    QCoreApplication::postEvent(q, new FeedbackEvent(q, callback, success, error));
}

}

using namespace icdmeta;

// class JProtocol

JProtocol::JProtocol(const QString &identity, QObject *parent)
    : QObject(parent)
    , J_DPTR(new JProtocolPrivate(this))
{
    Q_D(JProtocol);
    d->identity = identity;
    d->init();
}

JProtocol::~JProtocol()
{
    Q_D(JProtocol);
    delete d;
}

void JProtocol::registerQmlType()
{
    //
    IcdMetaRegisterUncreatableType2(JProtocol);

    //
}

QString JProtocol::identity() const
{
    Q_D(const JProtocol);
    return d->identity;
}

QString JProtocol::domain() const
{
    Q_D(const JProtocol);
    return d->domain;
}

QString JProtocol::tableName() const
{
    Q_D(const JProtocol);
    return d->tableName;
}

QString JProtocol::filePath() const
{
    Q_D(const JProtocol);
    return d->filePath;
}

QString JProtocol::queryPath() const
{
    Q_D(const JProtocol);
    return d->queryPath;
}

QString JProtocol::channelId() const
{
    Q_D(const JProtocol);
    return d->channelId;
}

bool JProtocol::isValid() const
{
    Q_D(const JProtocol);
    return (d->table != nullptr);
}

bool JProtocol::isChecked() const
{
    Q_D(const JProtocol);
    if (d->table) {
        return d->table->isChecked();
    } else {
        return false;
    }
}

icdmeta::JIcdTable *JProtocol::table() const
{
    Q_D(const JProtocol);
    return d->table.data();
}

void JProtocol::setIdentity(const QString &value)
{
    Q_D(JProtocol);
    if (value != d->identity) {
        d->identity = value;
        emit identityChanged(value);
    }
}

void JProtocol::setDomain(const QString &value)
{
    Q_D(JProtocol);
    if (value != d->domain) {
        d->domain = value;
        emit domainChanged(value);
    }
}

void JProtocol::setTableName(const QString &value)
{
    Q_D(JProtocol);
    if (value != d->tableName) {
        d->tableName = value;
        emit tableNameChanged(value);
    }
}

void JProtocol::setFilePath(const QString &value)
{
    Q_D(JProtocol);
    if (value != d->filePath) {
        d->filePath = value;
        emit filePathChanged(value);
    }
}

void JProtocol::setQueryPath(const QString &value)
{
    Q_D(JProtocol);
    if (value != d->queryPath) {
        d->queryPath = value;
        emit queryPathChanged(value);
    }
}

void JProtocol::setChannelId(const QString &value)
{
    Q_D(JProtocol);
    if (value != d->channelId) {
        d->channelId = value;
        emit channelIdChanged(value);
    }
}

bool JProtocol::load(QJSValue callback)
{
    Q_D(JProtocol);
    return d->load(callback);
}

void JProtocol::unload()
{
    Q_D(JProtocol);
    d->unload();
}

void JProtocol::resetBuffer()
{
    Q_D(JProtocol);
    if (d->table) {
        d->table->resetData();
    }
}

void JProtocol::cancelWatcher()
{
    Q_D(JProtocol);
    while (!d->watchers.isEmpty()) {
        auto watcher = d->watchers.takeFirst();
        watcher->cancel();
        watcher->waitForFinished();
    }
    QCoreApplication::processEvents();
}

Json::Value JProtocol::save() const
{
    Q_D(const JProtocol);
    Json::Value json;
    json["id"] = d->identity.toStdString();
    json["domain"] = d->domain.toStdString();
    json["tableName"] = d->tableName.toStdString();
    json["filePath"] = d->filePath.toStdString();
    json["queryPath"] = d->queryPath.toStdString();
    json["channelId"] = d->channelId.toStdString();
    return json;
}

bool JProtocol::restore(const Json::Value &json, int)
{
    if (json.isMember("id")) {
        setIdentity(QString::fromStdString(json["id"].asString()));
    }
    setDomain(QString::fromStdString(json["domain"].asString()));
    setTableName(QString::fromStdString(json["tableName"].asString()));
    setFilePath(IcdCore::instance()->parsePath(QString::fromStdString(json["filePath"].asString()), true));
    setQueryPath(QString::fromStdString(json["queryPath"].asString()));
    setChannelId(QString::fromStdString(json["channelId"].asString()));

    return true;
}

void JProtocol::customEvent(QEvent *event)
{
    switch (static_cast<int>(event->type())) {
    case Event_ProtocolFeedback:
    {
        FeedbackEvent *fbEvent = reinterpret_cast<FeedbackEvent *>(event);
        if (!fbEvent) {
            break;
        }
        fbEvent->execute();
        break;
    }
    default:
        break;
    }
}
