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
    case Icd::ArrayI8: return IcdCore::ArrayI8;
    case Icd::ArrayU8: return IcdCore::ArrayU8;
    case Icd::ArrayI16: return IcdCore::ArrayI16;
    case Icd::ArrayU16: return IcdCore::ArrayU16;
    case Icd::ArrayI32: return IcdCore::ArrayI32;
    case Icd::ArrayU32: return IcdCore::ArrayU32;
    case Icd::ArrayI64: return IcdCore::ArrayI64;
    case Icd::ArrayU64: return IcdCore::ArrayU64;
    case Icd::ArrayF32: return IcdCore::ArrayF32;
    case Icd::ArrayF64: return IcdCore::ArrayF64;
    default: return IcdCore::ArrayInvalid;
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
