﻿#include "precomp.h"
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

int Root::rtti() const
{
    return ObjectRoot;
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
    if (index < 0 || index >= static_cast<int>(d->vehicles.size())) {
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
    return static_cast<int>(d->vehicles.size());
}

VehiclePtr Root::vehicleAt(int index) const
{
    if (index < 0 || index >= int(d->vehicles.size())) {
        return VehiclePtr();
    }

    return d->vehicles.at(static_cast<size_t>(index));
}

VehiclePtr Root::vehicleByName(const std::string &name) const
{
    for (VehiclePtrArray::const_iterator citer = d->vehicles.cbegin();
         citer != d->vehicles.end(); ++citer) {
        const VehiclePtr &vehicle = *citer;
        if(vehicle->name() == name) {
            return vehicle;
        }
    }

    return VehiclePtr();
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

    return VehiclePtr();
}

bool Root::isEmpty() const
{
    return d->vehicles.empty();
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

ObjectPtr Root::copy() const
{
    RootPtr newRoot = std::make_shared<Root>(*this);
    newRoot->setParent(nullptr);
    return newRoot;
}

ObjectPtr Root::clone() const
{
    RootPtr newRoot = std::make_shared<Root>(*this);
    newRoot->setParent(nullptr);
    // children
    const VehiclePtrArray &vehicles = d->vehicles;
    for (VehiclePtrArray::const_iterator citer = vehicles.cbegin();
         citer != vehicles.cend(); ++citer) {
        newRoot->appendVehicle(JHandlePtrCast<Vehicle>((*citer)->clone()));
    }
    return newRoot;
}

Root &Root::operator =(const Root &other)
{
    if (this == &other) {
        return *this;
    }
    Object::operator =(other);
    d->vehicles = other.allVehicle();
    return *this;
}

ObjectPtr Root::findByDomain(const std::string &domain, int domainType,
                             bool ignoreComplex) const
{
    if (domain.empty()) {
        return Icd::ObjectPtr();
    }

    std::string::size_type index = domain.find_first_of('/');
    std::string current, next;
    if (index == std::string::npos) {
        current = domain;
    } else {
        current = domain.substr(0, index);
        next = domain.substr(index + 1);
    }

    for (VehiclePtrArray::const_iterator citer = d->vehicles.cbegin();
         citer != d->vehicles.cend(); ++citer) {
        const VehiclePtr &vehicle = *citer;
        if (vehicle->domainOfType(domainType) == current) {
            if (next.empty()) {
                return vehicle;
            } else {
                Icd::ObjectPtr object = vehicle->findByDomain(next, domainType, ignoreComplex);
                if (object) {
                    return object;
                }
            }
        }
    }

    return nullptr;
}

bool Root::hasChildByName(const std::string &name, const Icd::ObjectPtr &exclude) const
{
    for (VehiclePtrArray::const_iterator citer = d->vehicles.cbegin();
         citer != d->vehicles.end(); ++citer) {
        const VehiclePtr &vehicle = *citer;
        if (exclude && vehicle->id() == exclude->id()) {
            continue;
        }
        if(vehicle->name() == name) {
            return true;
        }
    }

    return false;
}

bool Root::hasChildByMark(const std::string &mark, const Icd::ObjectPtr &exclude) const
{
    for (VehiclePtrArray::const_iterator citer = d->vehicles.cbegin();
         citer != d->vehicles.end(); ++citer) {
        const VehiclePtr &vehicle = *citer;
        if (exclude && vehicle->id() == exclude->id()) {
            continue;
        }
        if(vehicle->mark() == mark) {
            return true;
        }
    }

    return false;
}

ObjectPtr Root::childAt(icd_uint64 index) const
{
    return vehicleAt(int(index));
}

ObjectPtr Root::replaceChild(icd_uint64 index, ObjectPtr &other)
{
    if (index >= d->vehicles.size()) {
        return ObjectPtr();
    }

    if (!other || other->objectType() != ObjectVehicle) {
        return ObjectPtr();
    }

    VehiclePtrArray::iterator iter = d->vehicles.begin() + int(index);
    VehiclePtr old = *iter;
    *iter = JHandlePtrCast<Vehicle>(other);

    return old;
}

void Root::removeChild(icd_uint64 index)
{
    if (index >= d->vehicles.size()) {
        return;
    }

    d->vehicles.erase(d->vehicles.cbegin() + int(index));
}

void Root::clearChildren()
{
    d->vehicles.clear();
}

Json::Value Root::save() const
{
#if 0
    Json::Value json = Object::save();
#else
    Json::Value json(Json::objectValue);
#endif
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
#if 0
    if (!Object::restore(json, deep)) {
        return false;
    }
#endif
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
