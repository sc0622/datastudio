#include "precomp.h"
#include "jicdobject.h"
#include "icdcore/icd_object.h"

namespace icdmeta {

// class JIcdObjectPrivate

class JIcdObjectPrivate
{
public:
    JIcdObjectPrivate(JIcdObject *q)
        : J_QPTR(q)
        , checked(false)
    {

    }

    void init();

private:
    J_DECLARE_PUBLIC(JIcdObject)
    Icd::ObjectPtr data;
    bool checked;
};

void JIcdObjectPrivate::init()
{

}

// class JIcdObject

JIcdObject::JIcdObject(const Icd::ObjectPtr &data, QObject *parent)
    : QObject(parent)
    , J_DPTR(new JIcdObjectPrivate(this))
{
    Q_D(JIcdObject);
    d->data = data;
    d->init();
}

JIcdObject::~JIcdObject()
{
    Q_D(JIcdObject);
    delete d;
}

void JIcdObject::registerQmlType()
{
    IcdMetaRegisterUncreatableType2(JIcdObject);
}

Icd::ObjectPtr JIcdObject::metaData() const
{
    Q_D(const JIcdObject);
    return d->data;
}

int JIcdObject::objectType() const
{
    Q_D(const JIcdObject);
    switch (d->data->objectType()) {
    case Icd::ObjectRoot: return IcdCore::ObjectRoot;
    case Icd::ObjectVehicle: return IcdCore::ObjectVehicle;
    case Icd::ObjectSystem: return IcdCore::ObjectSystem;
    case Icd::ObjectTable: return IcdCore::ObjectTable;
    case Icd::ObjectItem: return IcdCore::ObjectItem;
    default: return IcdCore::ObjectInvalid;
    };
}

QString JIcdObject::objectTypeString() const
{
    Q_D(const JIcdObject);
    return QString::fromStdString(d->data->objectTypeString());
}

QString JIcdObject::id() const
{
    Q_D(const JIcdObject);
    return QString::fromStdString(d->data->id());
}

QString JIcdObject::name() const
{
    Q_D(const JIcdObject);
    return QString::fromStdString(d->data->name());
}

QString JIcdObject::mark() const
{
    Q_D(const JIcdObject);
    return QString::fromStdString(d->data->mark());
}

QString JIcdObject::desc() const
{
    Q_D(const JIcdObject);
    return QString::fromStdString(d->data->desc());
}

bool JIcdObject::isChecked() const
{
    Q_D(const JIcdObject);
    return d->checked;
}

bool JIcdObject::isPrivateMark() const
{
    Q_D(const JIcdObject);
    return d->data->isPrivateMark();
}

QString JIcdObject::domainOfType(int domainType) const
{
    switch (domainType) {
    case IcdCore::DomainId: return id();
    case IcdCore::DomainName: return name();
    case IcdCore::DomainMark: return mark();
    default: return "";
    }
}

int JIcdObject::childCount() const
{
    return 0;
}

QString JIcdObject::text() const
{
    return name();
}

QString JIcdObject::valueString() const
{
    return name();
}

void JIcdObject::updateData()
{
    emit textChanged();
    emit valueStringChanged();
}

void JIcdObject::resetData()
{
    Q_D(JIcdObject);
    d->data->resetData();
}

void JIcdObject::clearData()
{
    Q_D(JIcdObject);
    d->data->clearData();
}

void JIcdObject::setId(const QString &value)
{
    Q_D(JIcdObject);
    if (value != QString::fromStdString(d->data->id())) {
        d->data->setId(value.toStdString());
        emit idChanged(value);
    }
}

void JIcdObject::setName(const QString &value)
{
    Q_D(JIcdObject);
    if (value != QString::fromStdString(d->data->name())) {
        d->data->setName(value.toStdString());
        emit nameChanged(value);
    }
}

void JIcdObject::setMark(const QString &value)
{
    Q_D(JIcdObject);
    if (value != QString::fromStdString(d->data->mark())) {
        d->data->setMark(value.toStdString());
        emit markChanged(value);
    }
}

void JIcdObject::setDesc(const QString &value)
{
    Q_D(JIcdObject);
    if (value != QString::fromStdString(d->data->desc())) {
        d->data->setDesc(value.toStdString());
        emit descChanged(value);
    }
}

void JIcdObject::setChecked(bool value)
{
    Q_D(JIcdObject);
    if (value != d->checked) {
        d->checked = value;
        emit checkedChanged(value);
    }
}

} // end of namespace icdmeta
