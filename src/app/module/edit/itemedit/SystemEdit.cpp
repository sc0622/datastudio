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

bool SystemEdit::init()
{
    if (!ObjectEdit::init()) {
        return false;
    }

    SystemEdit::restoreContent(false);

    return true;
}

void SystemEdit::restoreContent(bool recursive)
{
    if (recursive) {
        ObjectEdit::restoreContent(recursive);
    }

    lock();

    const Icd::SystemPtr system = this->system();
    if (system) {
        //
    }

    unlock();
}

bool SystemEdit::validate()
{
    if (!ObjectEdit::validate()) {
        return false;
    }

    //TODO

    return true;
}

void SystemEdit::saveContent()
{
    ObjectEdit::saveContent();

    //TODO
}

}
