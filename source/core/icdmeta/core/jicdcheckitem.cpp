#include "precomp.h"
#include "jicdcheckitem.h"

namespace icdmeta {

// class JIcdCheckItemPrivate

class JIcdCheckItemPrivate
{
public:
    JIcdCheckItemPrivate(JIcdCheckItem *q)
        : q_ptr(q)
        , data(Q_NULLPTR)
    {

    }

    void init();

private:
    J_DECLARE_PUBLIC(JIcdCheckItem)
    Icd::CheckItemPtr data;
};

void JIcdCheckItemPrivate::init()
{

}

// class JIcdCheckItem

JIcdCheckItem::JIcdCheckItem(const Icd::CheckItemPtr &data, QObject *parent)
    : JIcdItem(data, parent)
    , d_ptr(new JIcdCheckItemPrivate(this))
{
    Q_D(JIcdCheckItem);
    d->data = data;
    d->init();
}

JIcdCheckItem::~JIcdCheckItem()
{
    Q_D(JIcdCheckItem);
    delete d;
}

void JIcdCheckItem::registerQmlType()
{
    //
    jRegisterUncreatableType(JIcdCheckItem);

    //
}

Icd::CheckItemPtr JIcdCheckItem::metaData() const
{
    Q_D(const JIcdCheckItem);
    return d->data;
}

IcdCore::CheckType JIcdCheckItem::checkType() const
{
    Q_D(const JIcdCheckItem);
    return (IcdCore::CheckType)d->data->checkType();
}

int JIcdCheckItem::startPos() const
{
    Q_D(const JIcdCheckItem);
    return d->data->startPos();
}

int JIcdCheckItem::endPos() const
{
    Q_D(const JIcdCheckItem);
    return d->data->endPos();
}

int JIcdCheckItem::checkLength() const
{
    Q_D(const JIcdCheckItem);
    return d->data->checkLength();
}

bool JIcdCheckItem::isValid() const
{
    Q_D(const JIcdCheckItem);
    return d->data->isValid();
}

QString JIcdCheckItem::typeName() const
{
    Q_D(const JIcdCheckItem);
    return QString::fromStdString(d->data->typeName());
}

QString JIcdCheckItem::checkTypeString() const
{
    Q_D(const JIcdCheckItem);
    return QString::fromStdString(d->data->checkTypeString());
}

QString JIcdCheckItem::checkTypeString(IcdCore::CheckType type)
{
    return QString::fromStdString(Icd::CheckItem::checkTypeString((Icd::CheckType)type));
}

IcdCore::CheckType JIcdCheckItem::stringCheckType(const QString &str)
{
    return (IcdCore::CheckType)Icd::CheckItem::stringCheckType(str.toStdString());
}

QString JIcdCheckItem::text() const
{
    Q_D(const JIcdCheckItem);
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

QString JIcdCheckItem::valueString() const
{
    Q_D(const JIcdCheckItem);
    return QString("%1").arg((qulonglong)d->data->data(), 0, 10);
}

QString JIcdCheckItem::fullValue() const
{
    Q_D(const JIcdCheckItem);
    return QString("%1")
            .arg((uint)d->data->data(),
                 int(d->data->bufferSize() * 2), 16, QChar('0'))
            .toUpper();
}

} // end of namespace icdmeta
