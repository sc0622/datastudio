#ifndef JLOGINMODEL_H
#define JLOGINMODEL_H

#include "../icdmeta_global.h"

namespace icdmeta {

class JLoginModelPrivate;

class ICDMETA_EXPORT JLoginModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString userName READ userName WRITE setUserName NOTIFY userNameChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(bool accepted READ accepted WRITE setAccepted NOTIFY acceptedChanged)
public:
    explicit JLoginModel(QObject *parent = nullptr);
    ~JLoginModel();

    static void registerQmlType();

    QString userName() const;
    QString password() const;
    bool accepted();

    Q_INVOKABLE bool login();
    Q_INVOKABLE bool logout();

signals:
    void userNameChanged(const QString &value);
    void passwordChanged(const QString &value);
    void acceptedChanged(bool value);

public slots:
    void setUserName(const QString &value);
    void setPassword(const QString &value);
    void setAccepted(bool value);
    void reset();

private:
    Q_DISABLE_COPY(JLoginModel)
    J_DECLARE_PRIVATE(JLoginModel)
    J_DECLARE_SINGLE_INSTANCE(JLoginModel)
};

}

#endif // JLOGINMODEL_H
