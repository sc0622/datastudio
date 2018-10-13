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

QString JIcdArrayItem::toString() const
{
    Q_D(const JIcdArrayItem);
    return  QString::fromStdString(d->data->toString());
}

qint8 *JIcdArrayItem::i8() const
{
    Q_D(const JIcdArrayItem);
    return  d->data->i8();
}

quint8 *JIcdArrayItem::u8() const
{
    Q_D(const JIcdArrayItem);
    return  d->data->u8();
}

qint16 *JIcdArrayItem::i16() const
{
    Q_D(const JIcdArrayItem);
    return  d->data->i16();
}

quint16 *JIcdArrayItem::u16() const
{
    Q_D(const JIcdArrayItem);
    return  d->data->u16();
}

qint32 *JIcdArrayItem::i32() const
{
    Q_D(const JIcdArrayItem);
    return  d->data->i32();
}

quint32 *JIcdArrayItem::u32() const
{
    Q_D(const JIcdArrayItem);
    return  d->data->u32();
}

qint64 *JIcdArrayItem::i64() const
{
    Q_D(const JIcdArrayItem);
    return  d->data->i64();
}

quint64 *JIcdArrayItem::u64() const
{
    Q_D(const JIcdArrayItem);
    return  d->data->u64();
}

float *JIcdArrayItem::f32() const
{
    Q_D(const JIcdArrayItem);
    return  d->data->f32();
}

double *JIcdArrayItem::f64() const
{
    Q_D(const JIcdArrayItem);
    return  d->data->f64();
}

double JIcdArrayItem::valueOf(int index) const
{
    Q_D(const JIcdArrayItem);
    return d->data->valueOf(index);
}

void JIcdArrayItem::setData(const char *buffer, int size)
{
    Q_D(JIcdArrayItem);
    d->data->setValue(buffer, size);
}

void JIcdArrayItem::setData(const QByteArray &data)
{
    Q_D(JIcdArrayItem);
    d->data->setValue(data.constData(), data.size());
}

void JIcdArrayItem::setData(const QString &text)
{
    setData(text.toLocal8Bit());
}

} // end of namespace icdmeta
