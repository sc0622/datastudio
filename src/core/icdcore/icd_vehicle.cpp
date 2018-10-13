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
    SystemPtrArray systems;
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

int Vehicle::rtti() const
{
    return ObjectVehicle;
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
    d->systems.push_back(system);
}

void Vehicle::insertSystem(int index, const SystemPtr &system)
{
    index = jBound(0, index, int(d->systems.size()));
    d->systems.insert(d->systems.cbegin() + index, system);
}

void Vehicle::removeSystem(int index)
{
    if (index < 0 || index >= int(d->systems.size())) {
        return;
    }

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
    return int(d->systems.size());
}

SystemPtr Vehicle::systemAt(int index) const
{
    if (index < 0 || index >= int(d->systems.size())) {
        return SystemPtr();
    }

    return d->systems.at(size_t(index));
}

SystemPtr Vehicle::systemByName(const std::string &name) const
{
    for (SystemPtrArray::const_iterator citer = d->systems.cbegin();
         citer != d->systems.end(); ++citer) {
        const SystemPtr &system = *citer;
        if(system->name() == name) {
            return system;
        }
    }

    return SystemPtr();
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

ObjectPtr Vehicle::copy() const
{
    VehiclePtr newVehicle = std::make_shared<Vehicle>(*this);
    return newVehicle;
}

ObjectPtr Vehicle::clone() const
{
    VehiclePtr newVehicle = std::make_shared<Vehicle>(*this);
    // children
    const SystemPtrArray &systems = d->systems;
    for (SystemPtrArray::const_iterator citer = systems.cbegin();
         citer != systems.cend(); ++citer) {
        SystemPtr newSystem = JHandlePtrCast<System>((*citer)->clone());
        newSystem->setParent(newVehicle.get());
        newVehicle->appendSystem(newSystem);
    }
    return newVehicle;
}

Vehicle &Vehicle::operator =(const Vehicle &other)
{
    if (this == &other) {
        return *this;
    }
    Object::operator =(other);
    d->systems = other.allSystem();
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

bool Vehicle::hasChildByName(const std::string &name, const Icd::ObjectPtr &exclude) const
{
    for (SystemPtrArray::const_iterator citer = d->systems.cbegin();
         citer != d->systems.end(); ++citer) {
        const SystemPtr &system = *citer;
        if (exclude && system->id() == exclude->id()) {
            continue;
        }
        if(system->name() == name) {
            return true;
        }
    }

    return false;
}

bool Vehicle::hasChildByMark(const std::string &mark, const Icd::ObjectPtr &exclude) const
{
    for (SystemPtrArray::const_iterator citer = d->systems.cbegin();
         citer != d->systems.end(); ++citer) {
        const SystemPtr &system = *citer;
        if (exclude && system->id() == exclude->id()) {
            continue;
        }
        if(system->mark() == mark) {
            return true;
        }
    }

    return false;
}

ObjectPtr Vehicle::childAt(icd_uint64 index) const
{
    return systemAt(int(index));
}

ObjectPtr Vehicle::replaceChild(icd_uint64 index, ObjectPtr &other)
{
    if (index >= d->systems.size()) {
        return ObjectPtr();
    }

    if (!other || other->objectType() != ObjectSystem) {
        return ObjectPtr();
    }

    SystemPtrArray::iterator iter = d->systems.begin() + int(index);
    SystemPtr old = *iter;
    *iter = JHandlePtrCast<System>(other);

    return old;
}

ObjectPtr Vehicle::replaceChild(const std::string &id, ObjectPtr &other)
{
    if (!other || other->objectType() != Icd::ObjectSystem) {
        return ObjectPtr();
    }

    const Icd::SystemPtr otherSystem = JHandlePtrCast<Icd::System>(other);
    if (!otherSystem) {
        return ObjectPtr();
    }

    for (SystemPtrArray::iterator iter = d->systems.begin();
         iter != d->systems.end(); ++iter) {
        const SystemPtr &system = *iter;
        if (system->id() != id) {
            continue;
        }
        *iter = otherSystem;
        return system;
    }

    return ObjectPtr();
}

void Vehicle::moveChild(int sourceIndex, int targetIndex)
{
    if (sourceIndex == targetIndex) {
        return;
    }

    if (sourceIndex < 0 || sourceIndex >= int(d->systems.size())) {
        return;
    }

    if (targetIndex < 0 || targetIndex >= int(d->systems.size())) {
        return;
    }

    SystemPtrArray::const_iterator citer = d->systems.cbegin() + sourceIndex;
    SystemPtr system = *citer;
    d->systems.erase(citer);
    d->systems.insert(d->systems.begin() + targetIndex, system);
}

void Vehicle::removeChild(icd_uint64 beginIndex, int endIndex)
{
    if (beginIndex >= d->systems.size()) {
        return;
    }

    if (endIndex >= 0 && int(beginIndex) < endIndex && endIndex < int(d->systems.size())) {
        d->systems.erase(d->systems.cbegin() + int(beginIndex),
                         d->systems.cbegin() + endIndex);
    } else {
        d->systems.erase(d->systems.cbegin() + int(beginIndex));
    }
}

void Vehicle::removeChild(const std::list<icd_uint64> &indexes)
{
    std::list<icd_uint64> sortedIndexes = indexes;
    sortedIndexes.sort(std::greater<icd_uint64>());

    for (std::list<icd_uint64>::const_iterator citer = sortedIndexes.cbegin();
         citer != sortedIndexes.cend(); ++citer) {
        const icd_uint64 index = *citer;
        if (index < d->systems.size()) {
            d->systems.erase(d->systems.cbegin() + int(index));
        }
    }
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
            SystemPtr system = std::make_shared<System>(systemJson["id"].asString(), this);
            if (!system->restore(systemJson, deep)) {
                continue;
            }
            appendSystem(system);
        }
    }

    return true;
}

} // end of namespace Icd
