#include "precomp.h"
#include "jicdnumericitem.h"
#include "jicdlimititem.h"

namespace icdmeta {

// class JIcdNumericItemPrivate

class JIcdNumericItemPrivate
{
public:
    JIcdNumericItemPrivate(JIcdNumericItem *q)
        : q_ptr(q)
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
    , d_ptr(new JIcdNumericItemPrivate(this))
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
    jRegisterUncreatableType(JIcdNumericItem);

    //
    JIcdLimitItem::registerQmlType();
}

Icd::NumericItemPtr JIcdNumericItem::metaData() const
{
    Q_D(const JIcdNumericItem);
    return d->data;
}

QString JIcdNumericItem::dataString() const
{
    Q_D(const JIcdNumericItem);
    return QString::fromStdString(d->data->dataString());
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
    return d->data->specs().size();
}

QString JIcdNumericItem::typeName() const
{
    Q_D(const JIcdNumericItem);
    return QString::fromStdString(d->data->typeName());
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

double JIcdNumericItem::orignalData() const
{
    Q_D(const JIcdNumericItem);
    return d->data->orignalData();
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
    qulonglong data = (qulonglong)((d->data->data() - d->data->offset())
                                   / d->data->scale());
    data &= (1ui64 << ((int)d->data->bufferSize() << 3)) - 1;
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

} // end of namespace icdmeta
