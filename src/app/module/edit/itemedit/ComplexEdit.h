#ifndef COMPLEX_EDIT_H
#define COMPLEX_EDIT_H

#include "ItemEdit.h"

namespace Icd {
class ComplexItem;
typedef std::shared_ptr<ComplexItem> ComplexItemPtr;
}

namespace Edit {

// class ComplexEdit

class ComplexEdit : public ItemEdit
{
    Q_OBJECT
public:
    explicit ComplexEdit(const Icd::ComplexItemPtr &complex, QWidget *parent = nullptr);
    ~ComplexEdit();

    Icd::ComplexItemPtr complex() const;

signals:

public slots:

private:
};

}

#endif // COMPLEX_EDIT_H
