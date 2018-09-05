#ifndef NUMERIC_EDIT_H
#define NUMERIC_EDIT_H

#include "ItemEdit.h"

namespace Icd {
class NumericItem;
typedef std::shared_ptr<NumericItem> NumericItemPtr;
}

namespace Edit {

// class NumericEdit

class NumericEdit : public ItemEdit
{
    Q_OBJECT
public:
    explicit NumericEdit(const Icd::NumericItemPtr &numeric, QWidget *parent = nullptr);
    ~NumericEdit();

    Icd::NumericItemPtr numeric() const;

signals:

public slots:

private:
};

}

#endif // NUMERIC_EDIT_H
