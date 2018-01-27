#include "precomp.h"
#include "jicdcounteritem.h"

namespace icdmeta {

// class JIcdCounterItemPrivate

class JIcdCounterItemPrivate
{
public:
    JIcdCounterItemPrivate(JIcdCounterItem *q)
        : q_ptr(q)
        , data(Q_NULLPTR)
    {

    }

    void init();

private:
    J_DECLARE_PUBLIC(JIcdCounterItem)
    Icd::CounterItemPtr data;
};

void JIcdCounterItemPrivate::init()
{

}

// class JIcdCounterItem

JIcdCounterItem::JIcdCounterItem(const Icd::CounterItemPtr &data, QObject *parent)
    : JIcdItem(data, parent)
    , d_ptr(new JIcdCounterItemPrivate(this))
{
    Q_D(JIcdCounterItem);
    d->data = data;
    d->init();
}

JIcdCounterItem::~JIcdCounterItem()
{
    Q_D(JIcdCounterItem);
    delete d;
}

void JIcdCounterItem::registerQmlType()
{
    //
    jRegisterUncreatableType(JIcdCounterItem);

    //
}

Icd::CounterItemPtr JIcdCounterItem::metaData() const
{
    Q_D(const JIcdCounterItem);
    return d->data;
}

uchar JIcdCounterItem::value() const
{
    Q_D(const JIcdCounterItem);
    return d->data->value();
}

QString JIcdCounterItem::dataString() const
{
    Q_D(const JIcdCounterItem);
    return QString::fromStdString(d->data->dataString());
}

QString JIcdCounterItem::counterTypeString() const
{
    Q_D(const JIcdCounterItem);
    return QString::fromStdString(d->data->counterTypeString());
}

QString JIcdCounterItem::counterTypeString(IcdCore::CounterType type)
{
    return QString::fromStdString(Icd::CounterItem::counterTypeString((Icd::CounterType)type));
}

IcdCore::CounterType JIcdCounterItem::stringCounterType(const QString &str)
{
    return (IcdCore::CounterType)Icd::CounterItem::stringCounterType(str.toStdString());
}

QString JIcdCounterItem::text() const
{
    Q_D(const JIcdCounterItem);
    QString info("<font bold=1 face=Consolas>0x");
    // data
    info.append(QString("%1")
                .arg((uint)d->data->data(),
                     int(d->data->bufferSize() * 2), 16, QChar('0'))
                .toUpper());
    // value
    info.append(", ");
    info.append(QString("%1").arg((qulonglong)d->data->data(), 0, 10));
    info.append("</font>");

    return info;
}

QString JIcdCounterItem::valueString() const
{
    Q_D(const JIcdCounterItem);
    return QString("%1").arg((qulonglong)d->data->data(), 0, 10);
}

QString JIcdCounterItem::fullValue() const
{
    Q_D(const JIcdCounterItem);
    return QString("%1")
            .arg((uint)d->data->data(),
                 int(d->data->bufferSize() * 2), 16, QChar('0'))
            .toUpper();
}

} // end of namespace icdmeta
