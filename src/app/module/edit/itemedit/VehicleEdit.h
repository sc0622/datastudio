#ifndef VEHICLE_EDIT_H
#define VEHICLE_EDIT_H

#include "ObjectEdit.h"

namespace Icd {
class Vehicle;
typedef std::shared_ptr<Vehicle> VehiclePtr;
}

namespace Edit {

// class VehicleEdit

class VehicleEdit : public ObjectEdit
{
    Q_OBJECT
public:
    explicit VehicleEdit(const Icd::VehiclePtr &vehicle, QWidget *parent = nullptr);
    ~VehicleEdit();

    Icd::VehiclePtr vehicle() const;

signals:

public slots:

private:
};

}

#endif // VEHICLE_EDIT_H
