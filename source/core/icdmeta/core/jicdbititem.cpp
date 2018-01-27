#include "precomp.h"
#include "jicdbititem.h"

namespace icdmeta {

// class JIcdBitItemPrivate

class JIcdBitItemPrivate
{
public:
    JIcdBitItemPrivate(JIcdBitItem *q)
        : q_ptr(q)
        , data(Q_NULLPTR)
    {

    }

    void init();

private:
    J_DECLARE_PUBLIC(JIcdBitItem)
    Icd::BitItemPtr data;
};

void JIcdBitItemPrivate::init()
{
    Q_Q(JIcdBitItem);
    QObject::connect(q, &JIcdBitItem::dataChanged, q, [=](qreal){
        emit q->currentSpecChanged(q->currentSpec());
    });
}

// class JIcdBitItem

JIcdBitItem::JIcdBitItem(const Icd::BitItemPtr &data, QObject *parent)
    : JIcdItem(data, parent)
    , d_ptr(new JIcdBitItemPrivate(this))
{
    Q_D(JIcdBitItem);
    d->data = data;
    d->init();
}

JIcdBitItem::~JIcdBitItem()
{
    Q_D(JIcdBitItem);
    delete d;
}

void JIcdBitItem::registerQmlType()
{
    //
    jRegisterUncreatableType(JIcdBitItem);

    //
}

Icd::BitItemPtr JIcdBitItem::metaData() const
{
    Q_D(const JIcdBitItem);
    return d->data;
}

int JIcdBitItem::bitStart() const
{
    Q_D(const JIcdBitItem);
    return d->data->bitStart();
}

int JIcdBitItem::bitCount() const
{
    Q_D(const JIcdBitItem);
    return d->data->bitCount();
}

int JIcdBitItem::typeSize() const
{
    Q_D(const JIcdBitItem);
    return d->data->typeSize();
}

int JIcdBitItem::specCount() const
{
    Q_D(const JIcdBitItem);
    return d->data->specs().size();
}

QString JIcdBitItem::currentSpec() const
{
    Q_D(const JIcdBitItem);
    return QString::fromStdString(d->data->specAt((quint64)data()));
}

QString JIcdBitItem::text() const
{
    Q_D(const JIcdBitItem);
    QString info("<font bold=1 face=Consolas>0x");
    // data
    info.append(QString("%1")
                .arg((qulonglong)d->data->data(), int(d->data->bufferSize() * 2), 16, QChar('0'))
                .toUpper());
    info.append(", ");
    // value
    switch (d->data->type()) {
    case Icd::ItemBitMap:
    {
        info.append(QString("%1").arg((qulonglong)d->data->data(),
                                      int(d->data->bufferSize() * 8), 2, QChar('0')));
        break;
    }
    case Icd::ItemBitValue:
    {
        const std::string spec = d->data->specAt(quint64(data()));
        if (spec.empty()) {
            info.append(JIcdItem::text());
        } else {
            info.append(QString::fromStdString(spec));
        }
        break;
    }
    default:
        break;
    }

    info.append("</font>");

    return info;
}

QString JIcdBitItem::valueString() const
{
    Q_D(const JIcdBitItem);
    QString info;
    switch (d->data->type()) {
    case Icd::ItemBitMap:
    {
        info = QString("%1").arg((qulonglong)d->data->data(),
                                 int(d->data->bufferSize() * 8), 2, QChar('0'));
        break;
    }
    case Icd::ItemBitValue:
    {
        const std::string spec = d->data->specAt(quint64(data()));
        if (spec.empty()) {
            //info.append(JIcdItem::text());
        } else {
            info.append(QString::fromStdString(spec));
        }
        return info;
    }
    default:
        break;
    }
    return info;
}

QString JIcdBitItem::fullValue() const
{
    Q_D(const JIcdBitItem);
    return QString("%1").arg((qulonglong)d->data->data(),
                             int(d->data->bufferSize() * 8), 2, QChar('0'));
}

bool JIcdBitItem::testBit(int offset) const
{
    Q_D(const JIcdBitItem);
    return d->data->testBit(offset);
}

quint64 JIcdBitItem::mask() const
{
    Q_D(const JIcdBitItem);
    return d->data->mask();
}

QString JIcdBitItem::specAt(quint64 key) const
{
    Q_D(const JIcdBitItem);
    return QString::fromStdString(d->data->specAt(key));
}

QString JIcdBitItem::nameAt(int offset) const
{
    Q_D(const JIcdBitItem);
    return QString::fromStdString(d->data->nameAt(offset));
}

QString JIcdBitItem::descAt(int offset) const
{
    Q_D(const JIcdBitItem);
    return QString::fromStdString(d->data->descAt(offset));
}

QString JIcdBitItem::typeName() const
{
    Q_D(const JIcdBitItem);
    return QString::fromStdString(d->data->typeName());
}

} // end of namespace icdmeta
