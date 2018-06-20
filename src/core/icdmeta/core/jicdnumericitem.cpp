#include "precomp.h"
#include "jicdnumericitem.h"
#include "icdcore/icd_item_numeric.h"
#include "jicdlimititem.h"

namespace icdmeta {

// class JIcdNumericItemPrivate

class JIcdNumericItemPrivate
{
public:
    JIcdNumericItemPrivate(JIcdNumericItem *q)
        : J_QPTR(q)
        , data(Q_NULLPTR)
    {

    }

    void init();

private:
    J_DECLARE_PUBLIC(JIcdNumericItem)
    Icd::NumericItemPtr data;
    QSharedPointer<JIcdLimitItem> limit;
};

void JIcdNumericItemPrivate::init()
{
    Q_Q(JIcdNumericItem);
    limit = QSharedPointer<JIcdLimitItem>(new JIcdLimitItem(data->limit(), q));
}

// class JIcdNumericItem

JIcdNumericItem::JIcdNumericItem(const Icd::NumericItemPtr &data, QObject *parent)
    : JIcdItem(data, parent)
    , J_DPTR(new JIcdNumericItemPrivate(this))
{
    Q_D(JIcdNumericItem);
    d->data = data;
    d->init();
}

JIcdNumericItem::~JIcdNumericItem()
{
    Q_D(JIcdNumericItem);
    delete d;
}

void JIcdNumericItem::registerQmlType()
{
    //
    IcdMetaRegisterUncreatableType2(JIcdNumericItem);

    //
    JIcdLimitItem::registerQmlType();
}

Icd::NumericItemPtr JIcdNumericItem::metaData() const
{
    Q_D(const JIcdNumericItem);
    return d->data;
}

IcdCore::NumericType JIcdNumericItem::numericType() const
{
    Q_D(const JIcdNumericItem);
    switch (d->data->numericType()) {
    case Icd::NumericI8: return IcdCore::NumericI8;
    case Icd::NumericU8: return IcdCore::NumericU8;
    case Icd::NumericI16: return IcdCore::NumericI16;
    case Icd::NumericU16: return IcdCore::NumericU16;
    case Icd::NumericI32: return IcdCore::NumericI32;
    case Icd::NumericU32: return IcdCore::NumericU32;
    case Icd::NumericI64: return IcdCore::NumericI64;
    case Icd::NumericU64: return IcdCore::NumericU64;
    case Icd::NumericF32: return IcdCore::NumericF32;
    case Icd::NumericF64: return IcdCore::NumericF64;
    default: return IcdCore::NumericInvalid;
    }
}

qreal JIcdNumericItem::scale() const
{
    Q_D(const JIcdNumericItem);
    return d->data->scale();
}

qreal JIcdNumericItem::offset() const
{
    Q_D(const JIcdNumericItem);
    return d->data->offset();
}

qreal JIcdNumericItem::decimals() const
{
    Q_D(const JIcdNumericItem);
    return d->data->decimals();
}

JIcdLimitItem *JIcdNumericItem::limit() const
{
    Q_D(const JIcdNumericItem);
    return d->limit.data();
}

QString JIcdNumericItem::unit() const
{
    Q_D(const JIcdNumericItem);
    return QString::fromStdString(d->data->unit());
}

int JIcdNumericItem::specCount() const
{
    Q_D(const JIcdNumericItem);
    return int(d->data->specs().size());
}

QString JIcdNumericItem::specAt(double key) const
{
    Q_D(const JIcdNumericItem);
    return QString::fromStdString(d->data->specAt(key));
}

qreal JIcdNumericItem::dataMinimum() const
{
    Q_D(const JIcdNumericItem);
    return d->data->dataRange().first;
}

qreal JIcdNumericItem::dataMaximum() const
{
    Q_D(const JIcdNumericItem);
    return d->data->dataRange().second;
}

qreal JIcdNumericItem::valueMinimum() const
{
    Q_D(const JIcdNumericItem);
    return d->data->valueRange().first;
}

qreal JIcdNumericItem::valueMaximum() const
{
    Q_D(const JIcdNumericItem);
    return d->data->valueRange().second;
}

double JIcdNumericItem::originalData() const
{
    Q_D(const JIcdNumericItem);
    return d->data->originalData();
}

bool JIcdNumericItem::outOfLimit() const
{
    Q_D(const JIcdNumericItem);
    return d->data->outOfLimit();
}

QString JIcdNumericItem::text() const
{
    Q_D(const JIcdNumericItem);
    QString info("<font bold=1 face=Consolas>0x");
    // data
    qulonglong data = qulonglong(((d->data->data() - d->data->offset())
                                   / d->data->scale()));
    data &= (1ull << (int(d->data->bufferSize()) << 3)) - 1;
    info.append(QString("%1")
                .arg(data, int(d->data->bufferSize() * 2), 16, QChar('0'))
                .toUpper());
    // value
    info.append(", ");
    info.append(QString("%1").arg(d->data->data(), 0, 'g', 20));
    info.append("</font>");
    const QString unit = this->unit();
    if (!unit.isEmpty()) {
        info.append(' ' % unit);
    }

    return info;
}

QString JIcdNumericItem::valueString() const
{
    Q_D(const JIcdNumericItem);
    return QString("%1").arg(d->data->data(), 0, 'g', 20);
}

QString JIcdNumericItem::fullValue() const
{
    Q_D(const JIcdNumericItem);
    QString text = QString("%1").arg(d->data->data(), 0, 'g', 20);
    const QString unit = this->unit();
    if (!unit.isEmpty()) {
        text.append(' ' % unit);
    }
    return text;
}

QString JIcdNumericItem::typeName() const
{
    Q_D(const JIcdNumericItem);
    return QString::fromStdString(d->data->typeName());
}

QString JIcdNumericItem::dataString() const
{
    Q_D(const JIcdNumericItem);
    return QString::fromStdString(d->data->dataString());
}

} // end of namespace icdmeta
