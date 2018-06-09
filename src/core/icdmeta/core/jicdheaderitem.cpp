#include "precomp.h"
#include "jicdheaderitem.h"
#include "icdcore/icd_item_header.h"

namespace icdmeta {

// class JIcdHeaderItemPrivate

class JIcdHeaderItemPrivate
{
public:
    JIcdHeaderItemPrivate(JIcdHeaderItem *q)
        : J_QPTR(q)
        , data(Q_NULLPTR)
    {

    }

    void init();

private:
    J_DECLARE_PUBLIC(JIcdHeaderItem)
    Icd::HeaderItemPtr data;
};

void JIcdHeaderItemPrivate::init()
{

}

// class JIcdHeaderItem

JIcdHeaderItem::JIcdHeaderItem(const Icd::HeaderItemPtr &data, QObject *parent)
    : JIcdItem(data, parent)
    , J_DPTR(new JIcdHeaderItemPrivate(this))
{
    Q_D(JIcdHeaderItem);
    d->data = data;
    d->init();
}

JIcdHeaderItem::~JIcdHeaderItem()
{
    Q_D(JIcdHeaderItem);
    delete d;
}

void JIcdHeaderItem::registerQmlType()
{
    IcdMetaRegisterUncreatableType2(JIcdHeaderItem);
}

Icd::HeaderItemPtr JIcdHeaderItem::metaData() const
{
    Q_D(const JIcdHeaderItem);
    return d->data;
}

QString JIcdHeaderItem::typeName() const
{
    Q_D(const JIcdHeaderItem);
    return QString::fromStdString(d->data->typeName());
}

uchar JIcdHeaderItem::value() const
{
    Q_D(const JIcdHeaderItem);
    return d->data->value();
}

QString JIcdHeaderItem::text() const
{
    Q_D(const JIcdHeaderItem);
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

QString JIcdHeaderItem::valueString() const
{
    Q_D(const JIcdHeaderItem);
    return QString("%1").arg((qulonglong)d->data->data(), 0, 10);
}

QString JIcdHeaderItem::fullValue() const
{
    Q_D(const JIcdHeaderItem);
    return QString("%1")
            .arg((uint)d->data->data(),
                 int(d->data->bufferSize() * 2), 16, QChar('0'))
            .toUpper();
}

QString JIcdHeaderItem::dataString() const
{
    Q_D(const JIcdHeaderItem);
    return QString::fromStdString(d->data->dataString());
}

} // end of namespace icdmeta
