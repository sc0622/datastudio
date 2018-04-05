#include "precomp.h"
#include "jicditem.h"
#include "icdcore/icd_item.h"
#include "jicdheaderitem.h"
#include "jicdcounteritem.h"
#include "jicdcheckitem.h"
#include "jicdframecodeitem.h"
#include "jicdnumericitem.h"
#include "jicdbititem.h"
#include "jicdcomplexitem.h"
#include "jicdframeitem.h"

namespace icdmeta {

// class JIcdItemPrivate

class JIcdItemPrivate
{
public:
    JIcdItemPrivate(JIcdItem *q)
        : q_ptr(q)
        , data(Q_NULLPTR)
    {

    }

    void init();

private:
    J_DECLARE_PUBLIC(JIcdItem)
    Icd::ItemPtr data;
};

void JIcdItemPrivate::init()
{

}

// class JIcdItem

JIcdItem::JIcdItem(const Icd::ItemPtr &data, QObject *parent)
    : JIcdObject(data, parent)
    , d_ptr(new JIcdItemPrivate(this))
{
    Q_D(JIcdItem);
    d->data = data;
    d->init();
}

void JIcdItem::registerQmlType()
{
    //
    IcdMetaRegisterUncreatableType2(JIcdItem);

    //
    JIcdObject::registerQmlType();
    JIcdHeaderItem::registerQmlType();
    JIcdCounterItem::registerQmlType();
    JIcdCheckItem::registerQmlType();
    JIcdFrameCodeItem::registerQmlType();
    JIcdNumericItem::registerQmlType();
    JIcdBitItem::registerQmlType();
    JIcdComplexItem::registerQmlType();
    JIcdFrameItem::registerQmlType();
}

Icd::ItemPtr JIcdItem::metaData() const
{
    Q_D(const JIcdItem);
    return d->data;
}

IcdCore::ItemType JIcdItem::itemType() const
{
    Q_D(const JIcdItem);
    switch (d->data->type()) {
    case Icd::ItemHead: return IcdCore::ItemHead;
    case Icd::ItemCounter: return IcdCore::ItemCounter;
    case Icd::ItemCheck: return IcdCore::ItemCheck;
    case Icd::ItemFrameCode: return IcdCore::ItemFrameCode;
    case Icd::ItemNumeric: return IcdCore::ItemNumeric;
    case Icd::ItemBitMap: return IcdCore::ItemBitMap;
    case Icd::ItemBitValue: return IcdCore::ItemBitValue;
    case Icd::ItemComplex: return IcdCore::ItemComplex;
    case Icd::ItemFrame: return IcdCore::ItemFrame;
    default: return IcdCore::ItemInvalid;
    };
}

int JIcdItem::itemOffset() const
{
    Q_D(const JIcdItem);
    return d->data->itemOffset();
}

qreal JIcdItem::bufferSize() const
{
    Q_D(const JIcdItem);
    return d->data->bufferSize();
}

qreal JIcdItem::bufferOffset() const
{
    Q_D(const JIcdItem);
    return d->data->bufferOffset();
}

qreal JIcdItem::data() const
{
    Q_D(const JIcdItem);
    return d->data->data();
}

double JIcdItem::defaultValue() const
{
    Q_D(const JIcdItem);
    return d->data->defaultValue();
}

QString JIcdItem::text() const
{
    return QString::number(data());
}

QString JIcdItem::valueString() const
{
    return QString::number(data());
}

QString JIcdItem::fullValue() const
{
    return QString::number(data());
}

QString JIcdItem::typeString() const
{
    Q_D(const JIcdItem);
    return QString::fromStdString(d->data->typeString());
}

QString JIcdItem::dataString() const
{
    Q_D(const JIcdItem);
    return QString::fromStdString(d->data->dataString());
}

QString JIcdItem::typeName() const
{
    Q_D(const JIcdItem);
    return QString::fromStdString(d->data->typeName());
}

QString JIcdItem::typeString(IcdCore::ItemType type)
{
    return QString::fromStdString(Icd::Item::typeString((Icd::ItemType)type));
}

IcdCore::ItemType JIcdItem::stringType(const QString &str)
{
    return (IcdCore::ItemType)Icd::Item::stringType(str.toStdString());
}

JIcdItem *JIcdItem::create(const Icd::ItemPtr &data, QObject *parent)
{
    if (!data) {
        return Q_NULLPTR;
    }

    switch (data->type()) {
    case Icd::ItemHead:
        return new JIcdHeaderItem(JHandlePtrCast<Icd::HeaderItem, Icd::Item>(data), parent);
    case Icd::ItemCounter:
        return new JIcdCounterItem(JHandlePtrCast<Icd::CounterItem, Icd::Item>(data), parent);
    case Icd::ItemCheck:
        return new JIcdCheckItem(JHandlePtrCast<Icd::CheckItem, Icd::Item>(data), parent);
    case Icd::ItemFrameCode:
        return new JIcdFrameCodeItem(JHandlePtrCast<Icd::FrameCodeItem, Icd::Item>(data), parent);
    case Icd::ItemNumeric:
        return new JIcdNumericItem(JHandlePtrCast<Icd::NumericItem, Icd::Item>(data), parent);
    case Icd::ItemBitMap:
    case Icd::ItemBitValue:
        return new JIcdBitItem(JHandlePtrCast<Icd::BitItem, Icd::Item>(data), parent);
    case Icd::ItemComplex:
        return new JIcdComplexItem(JHandlePtrCast<Icd::ComplexItem, Icd::Item>(data), parent);
    case Icd::ItemFrame:
        return new JIcdFrameItem(JHandlePtrCast<Icd::FrameItem, Icd::Item>(data), parent);
    default:
        break;
    }

    return Q_NULLPTR;
}

void JIcdItem::updateData()
{
    Q_D(JIcdItem);
    JIcdObject::updateData();
    emit dataChanged(d->data->data());
}

void JIcdItem::resetData()
{
    Q_D(JIcdItem);
    d->data->resetData();
}

void JIcdItem::clearData()
{
    Q_D(JIcdItem);
    d->data->clearData();
}

void JIcdItem::setDefaultValue(qreal value)
{
    Q_D(JIcdItem);
    d->data->setDefaultValue(value);
}

void JIcdItem::setData(qreal value)
{
    Q_D(JIcdItem);
    if (value != d->data->data()) {
        d->data->setData(value);
        emit dataChanged(value);
    }
}

} // end of namespace icdmeta
