#include "precomp.h"
#include "icd_root.h"
#include <assert.h>

namespace Icd {

class RootData
{
    friend class Root;
public:
    RootData()
    {

    }

private:
    VehiclePtrArray vehicles;  //
};

Root::Root(Object *parent)
    : Object(ObjectRoot, parent)
    , d(new RootData())
{

}

Root::Root(const std::string &id, Object *parent)
    : Object(id, ObjectRoot, parent)
    , d(new RootData())
{

}

Root::Root(const Root &other)
    : Object(other)
    , d(new RootData())
{
    *this = other;
}

Root::~Root()
{
    delete d;
}

VehiclePtrArray Root::allVehicle()
{
    return d->vehicles;
}

const VehiclePtrArray &Root::allVehicle() const
{
    return d->vehicles;
}

void Root::appendVehicle(const VehiclePtr &vehicle)
{
    vehicle->setParent(this);
    d->vehicles.push_back(vehicle);
}

void Root::insertVehicle(int index, const VehiclePtr &vehicle)
{
    index = jBound(0, index, int(d->vehicles.size()));
    vehicle->setParent(this);
    d->vehicles.insert(d->vehicles.cbegin() + index, vehicle);
}

void Root::removeVehicle(int index)
{
    if (index < 0 || index >= (int)d->vehicles.size()) {
        return;     // overflow
    }

    //
    d->vehicles.erase(d->vehicles.cbegin() + index);
}

void Root::removeVehicleByMark(const std::string &mark)
{
    for (VehiclePtrArray::const_iterator citer = d->vehicles.cbegin();
         citer != d->vehicles.cend(); ++citer) {
        const VehiclePtr &vehicle = *citer;
        if(vehicle->mark() == mark) {
            d->vehicles.erase(citer);
            break;
        }
    }
}

void Root::clearVehicle()
{
    d->vehicles.clear();
}

int Root::vehicleCount() const
{
    return (int)d->vehicles.size();
}

VehiclePtr Root::vehicleAt(int index) const
{
    if (index < 0 || index >= int(d->vehicles.size())) {
        return 0;
    }

    return d->vehicles.at(index);
}

VehiclePtr Root::vehicleByMark(const std::string &mark) const
{
    for (VehiclePtrArray::const_iterator citer = d->vehicles.cbegin();
         citer != d->vehicles.end(); ++citer) {
        const VehiclePtr &vehicle = *citer;
        if(vehicle->mark() == mark) {
            return vehicle;
        }
    }

    return 0;
}

int Root::childCount() const
{
    int count = int(d->vehicles.size());

    // children
    for (VehiclePtrArray::const_iterator citer = d->vehicles.cbegin();
         citer != d->vehicles.end(); ++citer) {
        count += (*citer)->childCount();
    }

    return count;
}

void Root::resetData()
{
    for (VehiclePtrArray::const_iterator citer = d->vehicles.cbegin();
         citer != d->vehicles.end(); ++citer) {
        (*citer)->resetData();
    }
}

void Root::clearData()
{
    for (VehiclePtrArray::const_iterator citer = d->vehicles.cbegin();
         citer != d->vehicles.end(); ++citer) {
        (*citer)->clearData();
    }
}

Object *Root::clone() const
{
    return new Root(*this);
}

Root &Root::operator =(const Root &other)
{
    Object::operator =(other);

    d->vehicles.clear();
    const VehiclePtrArray &vehicles = other.d->vehicles;
    for (VehiclePtrArray::const_iterator citer = vehicles.cbegin();
         citer != vehicles.cend(); ++citer) {
        appendVehicle(VehiclePtr(reinterpret_cast<Vehicle *>((*citer)->clone())));
    }

    return *this;
}

Json::Value Root::save() const
{
    Json::Value json = Object::save();

    if (!d->vehicles.empty()) {
        Json::Value vehiclesJson(Json::arrayValue);
        for (VehiclePtrArray::const_iterator citer = d->vehicles.cbegin();
             citer != d->vehicles.cend(); ++citer) {
            const VehiclePtr &vehicle = *citer;
            if (vehicle) {
                vehiclesJson.append(vehicle->save());
            }
        }
        json["vehicles"] = vehiclesJson;
    }

    return json;
}

bool Root::restore(const Json::Value &json, int deep)
{
    if (!Object::restore(json, deep)) {
        return false;
    }

    assert(deep <= Icd::ObjectItem);

    if (deep <= ObjectRoot) {
        return true;
    }

    clearVehicle();
    if (json.isMember("vehicles")) {
        Json::Value vehiclesJson = json["vehicles"];
        for (Json::ValueConstIterator citer = vehiclesJson.begin();
             citer != vehiclesJson.end(); ++citer) {
            const Json::Value &vehicleJson = *citer;
            VehiclePtr vehicle(new Vehicle(this));
            if (!vehicle->restore(vehicleJson, deep)) {
                continue;
            }
            appendVehicle(vehicle);
        }
    }

    return true;
}

} // end of namespace Icd
