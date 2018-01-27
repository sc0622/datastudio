#include "precomp.h"
#include "jicdcomplexitem.h"
#include "jicdtable.h"

namespace icdmeta {

// class JIcdComplexItemPrivate

class JIcdComplexItemPrivate
{
public:
    JIcdComplexItemPrivate(JIcdComplexItem *q)
        : q_ptr(q)
        , data(Q_NULLPTR)
        , table(Q_NULLPTR)
    {

    }

    void init();

private:
    J_DECLARE_PUBLIC(JIcdComplexItem)
    Icd::ComplexItemPtr data;
    QSharedPointer<JIcdTable> table;
};

void JIcdComplexItemPrivate::init()
{
    Q_Q(JIcdComplexItem);
    table = QSharedPointer<JIcdTable>(new JIcdTable(data->table(), q));
}

// class JIcdComplexItem

JIcdComplexItem::JIcdComplexItem(const Icd::ComplexItemPtr &data, QObject *parent)
    : JIcdItem(data, parent)
    , d_ptr(new JIcdComplexItemPrivate(this))
{
    Q_D(JIcdComplexItem);
    d->data = data;
    d->init();
}

JIcdComplexItem::~JIcdComplexItem()
{
    Q_D(JIcdComplexItem);
    delete d;
}

void JIcdComplexItem::registerQmlType()
{
    //
    jRegisterUncreatableType(JIcdComplexItem);

    //
}

Icd::ComplexItemPtr JIcdComplexItem::metaData() const
{
    Q_D(const JIcdComplexItem);
    return d->data;
}

int JIcdComplexItem::childCount() const
{
    Q_D(const JIcdComplexItem);
    return d->data->childCount();
}

JIcdTable *JIcdComplexItem::table() const
{
    Q_D(const JIcdComplexItem);
    return d->table.data();
}

QString JIcdComplexItem::text() const
{
    return "";
}

QString JIcdComplexItem::valueString() const
{
    return "";
}

QString JIcdComplexItem::typeName() const
{
    Q_D(const JIcdComplexItem);
    return QString::fromStdString(d->data->typeName());
}

JIcdObject *JIcdComplexItem::itemByMark(const QString &mark, bool deep) const
{
    Q_D(const JIcdComplexItem);
    return d->table->itemByMark(mark, deep);
}

JIcdTable *JIcdComplexItem::tableByMark(const QString &mark, bool deep) const
{
    Q_D(const JIcdComplexItem);
    return d->table->tableByMark(mark, deep);
}

JIcdObject *JIcdComplexItem::itemByDomain(const QString &domain, int domainType) const
{
    Q_D(const JIcdComplexItem);
    if (domain.isEmpty()) {
        return Q_NULLPTR;
    }

    const QString id = domain.section('/', 0, 0);
    if (id.isEmpty()) {
        return Q_NULLPTR;
    }
    const QString next = domain.section('/', 1);

    if (d->table->domainOfType(domainType) != id) {
        return Q_NULLPTR;
    }

    if (next.isEmpty()) {
        return d->table.data();
    } else {
        return d->table->itemByDomain(next, domainType);
    }
}

JIcdTable *JIcdComplexItem::tableByDomain(const QString &domain, int domainType) const
{
    JIcdObject *item = itemByDomain(domain, domainType);
    if (!item || item->objectType() != IcdCore::ObjectTable) {
        return Q_NULLPTR;
    }

    return qobject_cast<JIcdTable *>(item);
}

void JIcdComplexItem::updateData()
{
    Q_D(JIcdComplexItem);
    JIcdItem::updateData();
    d->table->updateData();
}

void JIcdComplexItem::resetData()
{
    Q_D(JIcdComplexItem);
    d->table->resetData();
}

void JIcdComplexItem::clearData()
{
    Q_D(JIcdComplexItem);
    d->table->clearData();
}

} // end of namespace icdmeta
