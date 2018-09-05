#include "precomp.h"
#include "icd_vehicle.h"
#include <assert.h>

namespace Icd {

class VehicleData
{
    friend class Vehicle;
public:
    VehicleData()
    {

    }

private:
    SystemPtrArray systems;  //
};

Vehicle::Vehicle(Object *parent)
    : Object(ObjectVehicle, parent)
    , d(new VehicleData())
{

}

Vehicle::Vehicle(const std::string &id, Object *parent)
    : Object(id, ObjectVehicle, parent)
    , d(new VehicleData())
{

}

Vehicle::Vehicle(const Vehicle &other)
    : Object(other)
    , d(new VehicleData())
{
    *this = other;
}

Vehicle::~Vehicle()
{
    delete d;
}

SystemPtrArray Vehicle::allSystem()
{
    return d->systems;
}

const SystemPtrArray &Vehicle::allSystem() const
{
    return d->systems;
}

void Vehicle::appendSystem(const SystemPtr &system)
{
    system->setParent(this);
    d->systems.push_back(system);
}

void Vehicle::insertSystem(int index, const SystemPtr &system)
{
    if (index < 0 || index >= static_cast<int>(d->systems.size())) {
        return;
    }

    system->setParent(this);
    d->systems.insert(d->systems.cbegin() + index, system);
}

void Vehicle::removeSystem(int index)
{
    if (index < 0 || index >= static_cast<int>(d->systems.size())) {
        return;     // overflow
    }

    //
    d->systems.erase(d->systems.cbegin() + index);
}

void Vehicle::removeSystemByMark(const std::string &mark)
{
    for (SystemPtrArray::const_iterator citer = d->systems.cbegin();
         citer != d->systems.cend(); ++citer) {
        const SystemPtr &system = *citer;
        if(system->mark() == mark) {
            d->systems.erase(citer);
            break;
        }
    }
}

void Vehicle::clearSystem()
{
    d->systems.clear();
}

int Vehicle::systemCount() const
{
    return static_cast<int>(d->systems.size());
}

SystemPtr Vehicle::systemAt(int index) const
{
    if (index < 0 || index >= static_cast<int>(d->systems.size())) {
        return SystemPtr();
    }

    return d->systems.at(static_cast<size_t>(index));
}

SystemPtr Vehicle::systemByMark(const std::string &mark) const
{
    for (SystemPtrArray::const_iterator citer = d->systems.cbegin();
         citer != d->systems.end(); ++citer) {
        const SystemPtr &system = *citer;
        if(system->mark() == mark) {
            return system;
        }
    }

    return SystemPtr();
}

bool Vehicle::isEmpty() const
{
    return d->systems.empty();
}

int Vehicle::childCount() const
{
    int count = int(d->systems.size());

    // children
    for (SystemPtrArray::const_iterator citer = d->systems.cbegin();
         citer != d->systems.end(); ++citer) {
        count += (*citer)->childCount();
    }

    return count;
}

void Vehicle::resetData()
{
    for (SystemPtrArray::const_iterator citer = d->systems.cbegin();
         citer != d->systems.end(); ++citer) {
        (*citer)->resetData();
    }
}

void Vehicle::clearData()
{
    for (SystemPtrArray::const_iterator citer = d->systems.cbegin();
         citer != d->systems.end(); ++citer) {
        (*citer)->clearData();
    }
}

Object *Vehicle::clone() const
{
    return new Vehicle(*this);
}

Vehicle &Vehicle::operator =(const Vehicle &other)
{
    Object::operator =(other);

    d->systems.clear();
    const SystemPtrArray &systems = other.d->systems;
    for (SystemPtrArray::const_iterator citer = systems.cbegin();
         citer != systems.cend(); ++citer) {
        appendSystem(SystemPtr(reinterpret_cast<System *>((*citer)->clone())));
    }

    return *this;
}

ObjectPtr Vehicle::findByDomain(const std::string &domain, int domainType,
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

    for (SystemPtrArray::const_iterator citer = d->systems.cbegin();
         citer != d->systems.cend(); ++citer) {
        const SystemPtr &system = *citer;
        if (system->domainOfType(domainType) == current) {
            if (next.empty()) {
                return system;
            } else {
                Icd::ObjectPtr object = system->findByDomain(next, domainType, ignoreComplex);
                if (object) {
                    return object;
                }
            }
        }
    }

    return nullptr;
}

void Vehicle::clearChildren()
{
    d->systems.clear();
}

Json::Value Vehicle::save() const
{
    Json::Value json = Object::save();

    if (!d->systems.empty()) {
        Json::Value systemsJson(Json::arrayValue);
        for (SystemPtrArray::const_iterator citer = d->systems.cbegin();
             citer != d->systems.cend(); ++citer) {
            const SystemPtr &system = *citer;
            if (system) {
                systemsJson.append(system->save());
            }
        }
        json["systems"] = systemsJson;
    }

    return json;
}

bool Vehicle::restore(const Json::Value &json, int deep)
{
    if (!Object::restore(json, deep)) {
        return false;
    }

    if (deep <= ObjectVehicle) {
        return true;
    }

    clearSystem();
    if (json.isMember("systems")) {
        Json::Value systemsJson = json["systems"];
        for (Json::ValueConstIterator citer = systemsJson.begin();
             citer != systemsJson.end(); ++citer) {
            const Json::Value &systemJson = *citer;
            SystemPtr system(new System(this));
            if (!system->restore(systemJson, deep)) {
                continue;
            }
            appendSystem(system);
        }
    }

    return true;
}

} // end of namespace Icd
