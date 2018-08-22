#ifndef JSEARCHEDIT_H
#define JSEARCHEDIT_H

#include <QWidget>
#include "icdwidget_global.h"

class QLineEdit;

namespace Icd {

class JSearchEditPrivate;

class ICDWIDGET_EXPORT JSearchEdit : public QWidget
{
    Q_OBJECT
public:
    explicit JSearchEdit(QWidget *parent = nullptr);
    ~JSearchEdit();

signals:
    void textChanged(const QString &text);

public slots:

private:
    Q_DISABLE_COPY(JSearchEdit)
    J_DECLARE_PRIVATE(JSearchEdit)
};

} // end of namespace Icd

#endif // JSEARCHEDIT_H
