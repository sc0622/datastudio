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

}
