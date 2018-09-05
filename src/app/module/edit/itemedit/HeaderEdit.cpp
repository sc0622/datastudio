#include "precomp.h"
#include "HeaderEdit.h"

namespace Edit {

// class HeaderEdit

HeaderEdit::HeaderEdit(const Icd::HeaderItemPtr &header, QWidget *parent)
    : ItemEdit(header, parent)
{

}

HeaderEdit::~HeaderEdit()
{

}

Icd::HeaderItemPtr HeaderEdit::header() const
{
    return JHandlePtrCast<Icd::HeaderItem>(item());
}

}
