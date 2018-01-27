#ifndef JTREECOMBOBOX_H
#define JTREECOMBOBOX_H

#include "jwt_global.h"
#include <QComboBox>
#include <QTreeWidget>

// - class JTreeComboBox -

class JWT_EXPORT JTreeComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit JTreeComboBox(QWidget *parent = 0);

Q_SIGNALS:

public Q_SLOTS:
};

#endif // JTREECOMBOBOX_H
