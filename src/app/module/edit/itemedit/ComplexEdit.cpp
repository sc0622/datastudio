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

bool ComplexEdit::init()
{
    if (!ItemEdit::init()) {
        return false;
    }

    ComplexEdit::restoreContent(false);

    return true;
}

void ComplexEdit::restoreContent(bool recursive)
{
    if (recursive) {
        ItemEdit::restoreContent(recursive);
    }

    lock();

    const Icd::ComplexItemPtr complex = this->complex();
    if (complex) {
        //
    }

    unlock();
}

bool ComplexEdit::validate() const
{
    if (!ItemEdit::validate()) {
        return false;
    }

    //

    return true;
}

void ComplexEdit::saveContent()
{
    ItemEdit::saveContent();
}

}
