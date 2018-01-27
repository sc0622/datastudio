#ifndef JIPADDRESSEDIT_H
#define JIPADDRESSEDIT_H

#include "jwt_global.h"
#include <QWidget>

// - class JIPAddressEdit -

class JIPAddressEditPrivate;

class JWT_EXPORT JIPAddressEdit : public QWidget
{
    Q_OBJECT
public:
    explicit JIPAddressEdit(QWidget *parent = 0);
    explicit JIPAddressEdit(const QString &text, QWidget *parent = 0);
    virtual ~JIPAddressEdit();

    QString text() const;

    bool isReadOnly() const;
    void setReadOnly(bool value);

Q_SIGNALS:
    void textChanged(const QString &text);

public Q_SLOTS:
    void setText(const QString &text);

private:
    Q_DISABLE_COPY(JIPAddressEdit)
    J_DECLARE_PRIVATE(JIPAddressEdit)
};

#endif // JIPADDRESSEDIT_H
