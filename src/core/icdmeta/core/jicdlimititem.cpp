#include "precomp.h"
#include "jicdlimititem.h"
#include "icdcore/icd_item_limit.h"

namespace icdmeta {

// class JIcdLimitItemPrivate

class JIcdLimitItemPrivate
{
public:
    JIcdLimitItemPrivate(JIcdLimitItem *q)
        : J_QPTR(q)
        , data(nullptr)
    {

    }

    void init();

private:
    J_DECLARE_PUBLIC(JIcdLimitItem)
    Icd::LimitItemPtr data;
};

void JIcdLimitItemPrivate::init()
{

}

// class JIcdLimitItem

JIcdLimitItem::JIcdLimitItem(const Icd::LimitItemPtr &data, QObject *parent)
    : QObject(parent)
    , J_DPTR(new JIcdLimitItemPrivate(this))
{
    Q_D(JIcdLimitItem);
    d->data = data;
    d->init();
}

JIcdLimitItem::~JIcdLimitItem()
{
    Q_D(JIcdLimitItem);
    delete d;
}

void JIcdLimitItem::registerQmlType()
{
    //
    IcdMetaRegisterUncreatableType2(JIcdLimitItem);

    //
}

Icd::LimitItemPtr JIcdLimitItem::metaData() const
{
    Q_D(const JIcdLimitItem);
    return d->data;
}

qreal JIcdLimitItem::minimum() const
{
    Q_D(const JIcdLimitItem);
    return d->data->minimum();
}

qreal JIcdLimitItem::maximum() const
{
    Q_D(const JIcdLimitItem);
    return d->data->maximum();
}

bool JIcdLimitItem::leftInf() const
{
    Q_D(const JIcdLimitItem);
    return d->data->minimumInf();
}

bool JIcdLimitItem::rightInf() const
{
    Q_D(const JIcdLimitItem);
    return d->data->maximumInf();
}

QString JIcdLimitItem::toString() const
{
    Q_D(const JIcdLimitItem);
    return QString::fromStdString(d->data->toString());
}

} // end of namespace icdmeta
