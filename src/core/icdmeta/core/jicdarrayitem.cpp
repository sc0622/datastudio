#include "precomp.h"
#include "jicdarrayitem.h"
#include "icdcore/icd_item_array.h"

namespace icdmeta {

// class JIcdArrayItemPrivate

class JIcdArrayItemPrivate
{
public:
    JIcdArrayItemPrivate(JIcdArrayItem *q)
        : J_QPTR(q)
        , data(nullptr)
    {

    }

    void init();

private:
    J_DECLARE_PUBLIC(JIcdArrayItem)
    Icd::ArrayItemPtr data;
};

void JIcdArrayItemPrivate::init()
{

}

// class JIcdArrayItem

JIcdArrayItem::JIcdArrayItem(const Icd::ArrayItemPtr &data, QObject *parent)
    : JIcdItem(data, parent)
    , J_DPTR(new JIcdArrayItemPrivate(this))
{
    Q_D(JIcdArrayItem);
    d->data = data;
    d->init();
}

JIcdArrayItem::~JIcdArrayItem()
{
    Q_D(JIcdArrayItem);
    delete d;
}

void JIcdArrayItem::registerQmlType()
{
    IcdMetaRegisterUncreatableType2(JIcdArrayItem);
}

Icd::ArrayItemPtr JIcdArrayItem::metaData() const
{
    Q_D(const JIcdArrayItem);
    return d->data;
}

QString JIcdArrayItem::typeName() const
{
    Q_D(const JIcdArrayItem);
    return QString::fromStdString(d->data->typeName());
}

QString JIcdArrayItem::text() const
{
    //Q_D(const JIcdArrayItem);
    QString info;
    //TODO
    return info;
}

QString JIcdArrayItem::valueString() const
{
    //Q_D(const JIcdArrayItem);
    //TODO
    return QString();
}

QString JIcdArrayItem::fullValue() const
{
    //Q_D(const JIcdArrayItem);
    //TODO
    return QString();
}

QString JIcdArrayItem::dataString() const
{
    Q_D(const JIcdArrayItem);
    return QString::fromStdString(d->data->dataString());
}

} // end of namespace icdmeta
