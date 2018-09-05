#include "precomp.h"
#include "NumericEdit.h"

namespace Edit {

// class NumericEdit

NumericEdit::NumericEdit(const Icd::NumericItemPtr &numeric, QWidget *parent)
    : ItemEdit(numeric, parent)
{

}

NumericEdit::~NumericEdit()
{

}

Icd::NumericItemPtr NumericEdit::numeric() const
{
    return JHandlePtrCast<Icd::NumericItem>(item());
}

}
