#include "precomp.h"
#include "SystemEdit.h"

namespace Edit {

// class SystemEdit

SystemEdit::SystemEdit(const Icd::SystemPtr &system, QWidget *parent)
    : ObjectEdit(system, parent)
{

}

SystemEdit::~SystemEdit()
{

}

Icd::SystemPtr SystemEdit::system() const
{
    return JHandlePtrCast<Icd::System>(object());
}

}
