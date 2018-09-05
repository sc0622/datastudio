#include "precomp.h"
#include "CounterEdit.h"

namespace Edit {

// class CounterEdit

CounterEdit::CounterEdit(const Icd::CounterItemPtr &counter, QWidget *parent)
    : ItemEdit(counter, parent)
{

}

CounterEdit::~CounterEdit()
{

}

Icd::CounterItemPtr CounterEdit::counter() const
{
    return JHandlePtrCast<Icd::CounterItem>(item());
}

}
