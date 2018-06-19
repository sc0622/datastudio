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

IcdCore::ArrayType JIcdArrayItem::arrayType() const
{
    Q_D(const JIcdArrayItem);
    switch (d->data->arrayType()) {
    case Icd::Int8Array: return IcdCore::Int8Array;
    case Icd::UInt8Array: return IcdCore::UInt8Array;
    case Icd::Int16Array: return IcdCore::Int16Array;
    case Icd::UInt16Array: return IcdCore::UInt16Array;
    case Icd::Int32Array: return IcdCore::Int32Array;
    case Icd::UInt32Array: return IcdCore::UInt32Array;
    case Icd::Int64Array: return IcdCore::Int64Array;
    case Icd::UInt64Array: return IcdCore::UInt64Array;
    case Icd::Float32Array: return IcdCore::Float32Array;
    case Icd::Float64Array: return IcdCore::Float64Array;
    default: return IcdCore::InvalidArray;
    }
}

int JIcdArrayItem::count() const
{
    Q_D(const JIcdArrayItem);
    return d->data->count();
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
