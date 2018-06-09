#include "precomp.h"
#include "jicddatetimeitem.h"

namespace icdmeta {

// class JIcdDateTimeItemPrivate

class JIcdDateTimeItemPrivate
{
public:
    JIcdDateTimeItemPrivate(JIcdDateTimeItem *q)
        : J_QPTR(q)
        , data(nullptr)
    {

    }

    void init();

private:
    J_DECLARE_PUBLIC(JIcdDateTimeItem)
    Icd::DateTimeItemPtr data;
};

void JIcdDateTimeItemPrivate::init()
{

}

// class JIcdDateTimeItem

JIcdDateTimeItem::JIcdDateTimeItem(const Icd::DateTimeItemPtr &data, QObject *parent)
    : JIcdItem(data, parent)
    , J_DPTR(new JIcdDateTimeItemPrivate(this))
{
    Q_D(JIcdDateTimeItem);
    d->data = data;
    d->init();
}

JIcdDateTimeItem::~JIcdDateTimeItem()
{
    Q_D(JIcdDateTimeItem);
    delete d;
}

void JIcdDateTimeItem::registerQmlType()
{
    IcdMetaRegisterUncreatableType2(JIcdDateTimeItem);
}

Icd::DateTimeItemPtr JIcdDateTimeItem::metaData() const
{
    Q_D(const JIcdDateTimeItem);
    return d->data;
}

QString JIcdDateTimeItem::typeName() const
{
    Q_D(const JIcdDateTimeItem);
    return QString::fromStdString(d->data->typeName());
}

QString JIcdDateTimeItem::text() const
{
    //Q_D(const JIcdDateTimeItem);
    QString info;
    //TODO
    return info;
}

QString JIcdDateTimeItem::valueString() const
{
    //Q_D(const JIcdDateTimeItem);
    //TODO
    return QString();
}

QString JIcdDateTimeItem::fullValue() const
{
    //Q_D(const JIcdDateTimeItem);
    //TODO
    return QString();
}

QString JIcdDateTimeItem::dataString() const
{
    Q_D(const JIcdDateTimeItem);
    return QString::fromStdString(d->data->dataString());
}

} // end of namespace icdmeta
