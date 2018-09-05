#include "precomp.h"
#include "CheckEdit.h"

namespace Edit {

// class CheckEdit

CheckEdit::CheckEdit(const Icd::CheckItemPtr &check, QWidget *parent)
    : ItemEdit(check, parent)
{

}

CheckEdit::~CheckEdit()
{

}

Icd::CheckItemPtr CheckEdit::check() const
{
    return JHandlePtrCast<Icd::CheckItem>(item());
}

}
