#include "precomp.h"
#include "ComplexEdit.h"

namespace Edit {

// class ComplexEdit

ComplexEdit::ComplexEdit(const Icd::ComplexItemPtr &complex, QWidget *parent)
    : ItemEdit(complex, parent)
{

}

ComplexEdit::~ComplexEdit()
{

}

Icd::ComplexItemPtr ComplexEdit::complex() const
{
    return JHandlePtrCast<Icd::ComplexItem>(item());
}

}
