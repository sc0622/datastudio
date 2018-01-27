#ifndef SEARCHEDIT_H
#define SEARCHEDIT_H

#include <QWidget>
#include "icdwidget_global.h"

class QLineEdit;

namespace Icd {

class SearchEditPrivate;

class ICDWIDGET_EXPORT SearchEdit : public QWidget
{
    Q_OBJECT
public:
    explicit SearchEdit(QWidget *parent = 0);

    ~SearchEdit();

signals:
    void textChanged(const QString &text);

public slots:

private:
    Q_DISABLE_COPY(SearchEdit)
    J_DECLARE_PRIVATE(SearchEdit)
};

} // end of namespace Icd

#endif // SEARCHEDIT_H
