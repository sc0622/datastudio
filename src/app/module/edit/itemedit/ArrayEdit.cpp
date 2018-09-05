#include "precomp.h"
#include "ArrayEdit.h"

namespace Edit {

// class ArrayEdit

ArrayEdit::ArrayEdit(const Icd::ArrayItemPtr &array, QWidget *parent)
    : ItemEdit(array, parent)
{

}

ArrayEdit::~ArrayEdit()
{

}

Icd::ArrayItemPtr ArrayEdit::array() const
{
    return JHandlePtrCast<Icd::ArrayItem>(item());
}

}
