#include "precomp.h"
#include "VehicleEdit.h"

namespace Edit {

// class VehicleEdit

VehicleEdit::VehicleEdit(const Icd::VehiclePtr &vehicle, QWidget *parent)
    : ObjectEdit(vehicle, parent)
{

}

VehicleEdit::~VehicleEdit()
{

}

Icd::VehiclePtr VehicleEdit::vehicle() const
{
    return JHandlePtrCast<Icd::Vehicle>(object());
}

bool VehicleEdit::init()
{
    if (!ObjectEdit::init()) {
        return false;
    }

    VehicleEdit::restoreContent(false);

    return true;
}

void VehicleEdit::restoreContent(bool recursive)
{
    if (recursive) {
        ObjectEdit::restoreContent(recursive);
    }

    lock();

    const Icd::VehiclePtr vehicle = this->vehicle();
    if (vehicle) {
        //
    }

    unlock();
}

bool VehicleEdit::validate()
{
    if (!ObjectEdit::validate()) {
        return false;
    }

    //

    return true;
}

void VehicleEdit::saveContent()
{
    ObjectEdit::saveContent();

    //
}

}
