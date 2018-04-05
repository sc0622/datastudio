#include "precomp.h"
#include "JProtocolPool.h"
#include "JProtocol.h"
#include "../core/jicdtable.h"
#include "JDataChannel.h"
#include "JDataChannelMgr.h"
#include "JFilterChannel.h"

namespace icdmeta {

// class JProtocolPoolPrivate

class JProtocolPoolPrivate
{
public:
    JProtocolPoolPrivate(JProtocolPool *q)
        : q_ptr(q)
    {

    }

    void init();
    static int protocolCount(QQmlListProperty<JProtocol> *property);
    static JProtocol *protocolAt(QQmlListProperty<JProtocol> *property, int index);

private:
    J_DECLARE_PUBLIC(JProtocolPool)
    QList<QSharedPointer<JProtocol> > protocols;
};

void JProtocolPoolPrivate::init()
{

}

int JProtocolPoolPrivate::protocolCount(QQmlListProperty<JProtocol> *property)
{
    JProtocolPool *q = qobject_cast<JProtocolPool *>(property->object);
    return q->d_ptr->protocols.count();
}

JProtocol *JProtocolPoolPrivate::protocolAt(QQmlListProperty<JProtocol> *property, int index)
{
    JProtocolPool *q = qobject_cast<JProtocolPool *>(property->object);
    return q->d_ptr->protocols[index].data();
}

}

using namespace icdmeta;

// class JProtocolPool

J_QML_IMPLEMENT_SINGLE_INSTANCE(JProtocolPool, QQmlEngine::CppOwnership, IcdCore)

JProtocolPool::JProtocolPool(QObject *parent)
    : QObject(parent)
    , d_ptr(new JProtocolPoolPrivate(this))
{
    Q_D(JProtocolPool);
    d->init();
}

JProtocolPool::~JProtocolPool()
{
    Q_D(JProtocolPool);
    delete d;
}

void JProtocolPool::registerQmlType()
{
    IcdMetaRegisterSingletonType3(JProtocolPool);

    JProtocol::registerQmlType();
    JDataChannelMgr::registerQmlType();
    JFilterChannel::registerQmlType();
}

QQmlListProperty<JProtocol> JProtocolPool::protocols()
{
    return QQmlListProperty<JProtocol>(this, nullptr,
                                       &JProtocolPoolPrivate::protocolCount,
                                       &JProtocolPoolPrivate::protocolAt);
}

bool JProtocolPool::loadConfig(const QString &filePath, const QString &nodePath)
{
    Q_D(JProtocolPool);
    d->protocols.clear();
    emit protocolsChanged();

    const Json::Value protocolJson = Json::resolve(filePath.toStdString(), nodePath.toStdString());
    if (protocolJson == Json::Value::null || !protocolJson.isArray()) {
        return false;
    }

    foreach (auto itemJson, protocolJson) {
        if (!itemJson.isObject()) {
            continue;
        }
        const std::string id = itemJson["id"].asString();
        if (id.empty()) {
            continue;
        }
        QSharedPointer<JProtocol> newObject =
                QSharedPointer<JProtocol>(new JProtocol(QString::fromStdString(id), this),
                                          j_delete_qobject);
        newObject->restore(itemJson);
        d->protocols.append(newObject);
    }

    return true;
}

bool JProtocolPool::saveConfig(const QString &filePath, const QString &nodePath)
{
    Q_D(JProtocolPool);
    if (d->protocols.isEmpty()) {
        return true;
    }

    Json::Value json;

    foreach (auto &protocol, d->protocols) {
        if (protocol) {
            json.append(protocol->save());
        }
    }

    return Json::make(filePath.toStdString(), nodePath.toStdString(), json, true, false);
}

JProtocol *JProtocolPool::identityOf(const QString &identity) const
{
    Q_D(const JProtocolPool);
    QListIterator<QSharedPointer<JProtocol> > citer(d->protocols);
    while (citer.hasNext()) {
        QSharedPointer<JProtocol> item = citer.next();
        if (item->identity() == identity) {
            return item.data();
        }
    }

    return nullptr;
}

JProtocol *JProtocolPool::tableIdOf(const QString &identity) const
{
    Q_D(const JProtocolPool);
    QListIterator<QSharedPointer<JProtocol> > citer(d->protocols);
    while (citer.hasNext()) {
        QSharedPointer<JProtocol> item = citer.next();
        icdmeta::JIcdTable *table = item->table();
        if (table && table->id() == identity) {
            return item.data();
        }
    }

    return nullptr;
}

void JProtocolPool::reset()
{
    Q_D(JProtocolPool);
    JDataChannelMgr::instance()->reset();
    JFilterChannel::instance()->reset();
    d->protocols.clear();
}
