#include "precomp.h"
#include "JLoginModel.h"

namespace icdmeta {

// class JLoginModelPrivate

class JLoginModelPrivate
{
public:
    JLoginModelPrivate(JLoginModel *q)
        : J_QPTR(q)
        , accepted(false)
    {

    }

    void init();

private:
    J_DECLARE_PUBLIC(JLoginModel)
    QString userName;
    QString password;
    bool accepted;
};

void JLoginModelPrivate::init()
{

}

// class JLoginModel

J_QML_IMPLEMENT_SINGLE_INSTANCE(JLoginModel, QQmlEngine::CppOwnership, IcdCore)

JLoginModel::JLoginModel(QObject *parent)
    : QObject(parent)
    , J_DPTR(new JLoginModelPrivate(this))
{
    Q_D(JLoginModel);
    d->init();
}

JLoginModel::~JLoginModel()
{
    Q_D(JLoginModel);
    delete d;
}

void JLoginModel::registerQmlType()
{
    IcdMetaRegisterSingletonType3(JLoginModel);
}

QString JLoginModel::userName() const
{
    Q_D(const JLoginModel);
    return d->userName;
}

QString JLoginModel::password() const
{
    Q_D(const JLoginModel);
    return d->password;
}

bool JLoginModel::accepted()
{
    Q_D(const JLoginModel);
    return d->accepted;
}

bool JLoginModel::login()
{
    //Q_D(JLoginModel);

    //TODO

    setAccepted(true);

    return true;
}

bool JLoginModel::logout()
{
    //Q_D(JLoginModel);

    //TODO

    setAccepted(false);

    return true;
}

void JLoginModel::setUserName(const QString &value)
{
    Q_D(JLoginModel);
    if (value != d->userName) {
        d->userName = value;
        emit userNameChanged(value);
    }
}

void JLoginModel::setPassword(const QString &value)
{
    Q_D(JLoginModel);
    if (value != d->password) {
        d->password = value;
        emit passwordChanged(value);
    }
}

void JLoginModel::setAccepted(bool value)
{
    Q_D(JLoginModel);
    if (value != d->accepted) {
        d->accepted = value;
        emit acceptedChanged(value);
    }
}

void JLoginModel::reset()
{

}

}
