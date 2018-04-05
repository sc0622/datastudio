#include "precomp.h"
#include "jicdlimititem.h"

namespace icdmeta {

// class JIcdLimitItemPrivate

class JIcdLimitItemPrivate
{
public:
    JIcdLimitItemPrivate(JIcdLimitItem *q)
        : q_ptr(q)
        , data(Q_NULLPTR)
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
    , d_ptr(new JIcdLimitItemPrivate(this))
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
    return d->data->leftInf();
}

bool JIcdLimitItem::rightInf() const
{
    Q_D(const JIcdLimitItem);
    return d->data->rightInf();
}

QString JIcdLimitItem::toString() const
{
    Q_D(const JIcdLimitItem);
    return QString::fromStdString(d->data->toString());
}

} // end of namespace icdmeta
