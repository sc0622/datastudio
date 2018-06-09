#include "precomp.h"
#include "jicdstringitem.h"
#include "icdcore/icd_item_string.h"

namespace icdmeta {

// class JIcdStringItemPrivate

class JIcdStringItemPrivate
{
public:
    JIcdStringItemPrivate(JIcdStringItem *q)
        : J_QPTR(q)
        , data(nullptr)
    {

    }

    void init();

private:
    J_DECLARE_PUBLIC(JIcdStringItem)
    Icd::StringItemPtr data;
};

void JIcdStringItemPrivate::init()
{

}

// class JIcdStringItem

JIcdStringItem::JIcdStringItem(const Icd::StringItemPtr &data, QObject *parent)
    : JIcdItem(data, parent)
    , J_DPTR(new JIcdStringItemPrivate(this))
{
    Q_D(JIcdStringItem);
    d->data = data;
    d->init();
}

JIcdStringItem::~JIcdStringItem()
{
    Q_D(JIcdStringItem);
    delete d;
}

void JIcdStringItem::registerQmlType()
{
    IcdMetaRegisterUncreatableType2(JIcdStringItem);
}

Icd::StringItemPtr JIcdStringItem::metaData() const
{
    Q_D(const JIcdStringItem);
    return d->data;
}

QString JIcdStringItem::typeName() const
{
    Q_D(const JIcdStringItem);
    return QString::fromStdString(d->data->typeName());
}

QString JIcdStringItem::text() const
{
    //Q_D(const JIcdStringItem);
    QString info;
    //TODO
    return info;
}

QString JIcdStringItem::valueString() const
{
    //Q_D(const JIcdStringItem);
    //TODO
    return QString();
}

QString JIcdStringItem::fullValue() const
{
    //Q_D(const JIcdStringItem);
    //TODO
    return QString();
}

QString JIcdStringItem::dataString() const
{
    Q_D(const JIcdStringItem);
    return QString::fromStdString(d->data->dataString());
}

} // end of namespace icdmeta
