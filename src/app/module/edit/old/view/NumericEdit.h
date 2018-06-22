#ifndef NUMERIC_EDIT_H
#define NUMERIC_EDIT_H

#include "ItemEdit.h"
#include "KernelClass/icdnumericdata.h"

class NumericEdit : public ItemEdit
{
    Q_OBJECT
public:
    explicit NumericEdit(QWidget *parent = nullptr);

    int windowType() const override;

signals:

private slots:

private:
};

#endif // NUMERIC_EDIT_H
