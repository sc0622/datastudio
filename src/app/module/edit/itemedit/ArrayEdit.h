#ifndef ARRAY_EDIT_H
#define ARRAY_EDIT_H

#include "ItemEdit.h"

namespace Icd {
class ArrayItem;
typedef std::shared_ptr<ArrayItem> ArrayItemPtr;
}

namespace Edit {

// class ArrayEdit

class ArrayEdit : public ItemEdit
{
    Q_OBJECT
public:
    explicit ArrayEdit(const Icd::ArrayItemPtr &array, QWidget *parent = nullptr);
    ~ArrayEdit();

    Icd::ArrayItemPtr array() const;

signals:

public slots:

private:
};

}

#endif // ARRAY_EDIT_H
