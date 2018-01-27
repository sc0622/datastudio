#include "precomp.h"
#include "jicdframeitem.h"
#include "jicdtable.h"

namespace icdmeta {

// class JIcdFrameItemPrivate

class JIcdFrameItemPrivate
{
public:
    JIcdFrameItemPrivate(JIcdFrameItem *q)
        : q_ptr(q)
        , data(nullptr)
    {

    }

    void init();
    static int tableCount(QQmlListProperty<JIcdTable> *property);
    static JIcdTable *tableAt(QQmlListProperty<JIcdTable> *property, int index);

private:
    J_DECLARE_PUBLIC(JIcdFrameItem)
    Icd::FrameItemPtr data;
    QVector<QSharedPointer<JIcdTable>> tables;
};

void JIcdFrameItemPrivate::init()
{
    Q_Q(JIcdFrameItem);
    tables.clear();
    for (auto &table : data->allTable()) {
        tables.append(QSharedPointer<JIcdTable>(new JIcdTable(table.second, q)));
    }
    emit q->countChanged();
}

int JIcdFrameItemPrivate::tableCount(QQmlListProperty<JIcdTable> *property)
{
    JIcdFrameItem *q = qobject_cast<JIcdFrameItem *>(property->object);
    return q->d_ptr->tables.count();
}

JIcdTable *JIcdFrameItemPrivate::tableAt(QQmlListProperty<JIcdTable> *property, int index)
{
    JIcdFrameItem *q = qobject_cast<JIcdFrameItem *>(property->object);
    return q->d_ptr->tables[index].data();
}

// class JIcdFrameItem

JIcdFrameItem::JIcdFrameItem(const Icd::FrameItemPtr &data, QObject *parent)
    : JIcdItem(data, parent)
    , d_ptr(new JIcdFrameItemPrivate(this))
{
    Q_D(JIcdFrameItem);
    d->data = data;
    d->init();
}

JIcdFrameItem::~JIcdFrameItem()
{
    Q_D(JIcdFrameItem);
    delete d;
}

void JIcdFrameItem::registerQmlType()
{
    //
    jRegisterUncreatableType(JIcdFrameItem);

    //
}

Icd::FrameItemPtr JIcdFrameItem::metaData() const
{
    Q_D(const JIcdFrameItem);
    return d->data;
}

int JIcdFrameItem::count() const
{
    Q_D(const JIcdFrameItem);
    return d->data->tableCount();
}

int JIcdFrameItem::sequenceCount() const
{
    Q_D(const JIcdFrameItem);
    return d->data->sequenceCount();
}

QString JIcdFrameItem::dataString() const
{
    Q_D(const JIcdFrameItem);
    return QString::fromStdString(d->data->dataString());
}

QString JIcdFrameItem::typeName() const
{
    Q_D(const JIcdFrameItem);
    return QString::fromStdString(d->data->typeName());
}

int JIcdFrameItem::childCount() const
{
    Q_D(const JIcdFrameItem);
    return d->data->childCount();
}

QQmlListProperty<JIcdTable> JIcdFrameItem::tables()
{
    return QQmlListProperty<JIcdTable>(this, nullptr,
                                       &JIcdFrameItemPrivate::tableCount,
                                       &JIcdFrameItemPrivate::tableAt);
}

icdmeta::JIcdTable *JIcdFrameItem::itemAt(int index) const
{
    Q_D(const JIcdFrameItem);
    return d->tables[index].data();
}

JIcdObject *JIcdFrameItem::itemByMark(const QString &mark, bool deep) const
{
    Q_D(const JIcdFrameItem);
    for (const auto &table : d->tables) {
        if (table->mark() == mark) {
            return table.data();
        } else if (deep) {
            const auto &childItem = table->itemByMark(mark, deep);
            if (childItem) {
                return childItem;
            }
        }
    }

    return nullptr;
}

JIcdTable *JIcdFrameItem::tableByMark(const QString &mark, bool deep) const
{
    Q_D(const JIcdFrameItem);
    for (const auto &table : d->tables) {
        if (table->mark() == mark) {
            return table.data();
        } else if (deep) {
            const auto &childTable = table->tableByMark(mark, deep);
            if (childTable) {
                return childTable;
            }
        }
    }

    return Q_NULLPTR;
}

JIcdObject *JIcdFrameItem::itemByDomain(const QString &domain,
                                        int domainType) const
{
    Q_D(const JIcdFrameItem);
    if (domain.isEmpty()) {
        return nullptr;
    }

    const QString current = domain.section('/', 0, 0);
    if (current.isEmpty()) {
        return nullptr;
    }
    const QString nextDomain = domain.section('/', 1);

    for (const auto &table : d->tables) {
        if (table->domainOfType(domainType) != current) {
            continue;
        }
        if (nextDomain.isEmpty()) {
            return table.data();
        }
        const auto childItem = table->itemByDomain(nextDomain, domainType);
        if (childItem) {
            return childItem;
        }

        break;
    }

    return nullptr;
}

JIcdTable *JIcdFrameItem::tableByDomain(const QString &domain,
                                        int domainType) const
{
    JIcdObject *item = itemByDomain(domain, domainType);
    if (!item || item->objectType() != IcdCore::ObjectTable) {
        return nullptr;
    }

    return qobject_cast<JIcdTable *>(item);
}

QString JIcdFrameItem::text() const
{
    return QString();
}

QString JIcdFrameItem::valueString() const
{
    return QString();
}

void JIcdFrameItem::updateData()
{
    Q_D(JIcdFrameItem);
    JIcdItem::updateData();
    for (auto &table : d->tables) {
        table->updateData();
    }
}

void JIcdFrameItem::resetData()
{
    Q_D(JIcdFrameItem);
    for (auto &table : d->tables) {
        table->resetData();
    }
}

void JIcdFrameItem::clearData()
{
    Q_D(JIcdFrameItem);
    for (auto &table : d->tables) {
        table->clearData();
    }
}

} // end of namespace icdmeta
