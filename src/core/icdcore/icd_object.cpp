#include "precomp.h"
#include "icd_object.h"
#include <algorithm>
#include "icd_item.h"

namespace Icd {

// class ObjectData

#define ADJUST_INDICES(start, end, len) \
    if (end > len) { \
    end = len; \
} else if (end < 0) { \
    end += len; \
    if (end < 0) { \
    end = 0; \
} \
} \
    if (start < 0) { \
    start += len; \
    if (start < 0) { \
    start = 0; \
} \
}

class ObjectData
{
    friend class Object;
public:
    ObjectData(Object *parent)
        : parent(parent)
        , objectType(ObjectInvalid)
    {
        char c[8];
#ifdef _MSC_VER
        _ultoa_s(uid, c, 16);
#else
        _ultoa_(uid, c, 16);
#endif
        id = std::string(c);
        uid++;
    }

    bool startsWith(const std::string &str, const std::string &prefix, bool caseSensitivity = true);
    bool endsWith(const std::string &str, const std::string &suffix, bool caseSensitivity = true);

private:
    static unsigned long uid;
    Object *parent;
    ObjectType objectType;
    std::string id;
    std::string domain;
    std::string name;
    std::string mark;
    std::string desc;
};

unsigned long ObjectData::uid = 1;

bool ObjectData::startsWith(const std::string &str, const std::string &prefix, bool caseSensitivity)
{
    if (str.empty() || prefix.empty()) {
        return false;
    }

    if (caseSensitivity) {
        return (str.compare(0, prefix.size(), prefix) == 0);
    } else {
        std::string _str = str;
        std::string _prefix = prefix;
        std::transform(_str.cbegin(), _str.cend(), _str.begin(), tolower);
        std::transform(_prefix.cbegin(), _prefix.cend(), _prefix.begin(), tolower);
        return (_str.compare(0, _prefix.size(), _prefix) == 0);
    }
}

bool ObjectData::endsWith(const std::string &str, const std::string &suffix, bool caseSensitivity)
{
    if (str.empty() || suffix.empty()) {
        return false;
    }

    if (caseSensitivity) {
        return (str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0);
    } else {
        std::string _str = str;
        std::string _suffix = suffix;
        std::transform(_str.cbegin(), _str.cend(), _str.begin(), tolower);
        std::transform(_suffix.cbegin(), _suffix.cend(), _suffix.begin(), tolower);
        return (_str.compare(_str.size() - suffix.size(), _suffix.size(), _suffix) == 0);
    }
}

// class Object

Object::Object(ObjectType type, Object *parent)
    : d(new ObjectData(parent))
{
    d->objectType = type;
}

Object::Object(const std::string &id, ObjectType type, Object *parent)
    : d(new ObjectData(parent))
{
    d->objectType = type;
    d->id = id;
}

Object::Object(const Object &other)
    : d(new ObjectData(other.parent()))
{
    *this = other;
}

Object::~Object()
{
    delete d;
}

int Object::rtti() const
{
    return ObjectInvalid;
}

ObjectType Object::objectType() const
{
    return d->objectType;
}

std::string Object::id() const
{
    return d->id;
}

std::string Object::domain() const
{
    return d->domain;
}

void Object::setDomain(const std::string &domain)
{
    d->domain = domain;
}

void Object::setId(const std::string &id)
{
    d->id = id;
}

std::string Object::name() const
{
    return d->name;
}

void Object::setName(const std::string &name)
{
    d->name = name;
}

std::string Object::mark() const
{
    return d->mark;
}

void Object::setMark(const std::string &mark)
{
    d->mark = mark;
}

std::string Object::desc() const
{
    return d->desc;
}

void Object::setDesc(const std::string &desc)
{
    d->desc = desc;
}

bool Object::isPrivateMark() const
{
#if 0
    if (d->startsWith(d->mark, "_")) {
        return true;
    }
#else
    if (d->mark.empty()) {
        return false;
    } else if (d->mark.at(0) == '_') {
        return true;
    }
#endif

    return false;
}

std::string Object::objectTypeString() const
{
    switch (d->objectType) {
    case ObjectRoot: return "Root";
    case ObjectVehicle: return "Vehicle";
    case ObjectSystem: return "System";
    case ObjectTable: return "Table";
    case ObjectItem: return "Item";
    default: return "Invalid";
    }
}

std::string Object::domainOfType(int domainType) const
{
    switch (domainType) {
    case Icd::DomainId: return id();
    case Icd::DomainName: return name();
    case Icd::DomainMark: return mark();
    default: return "";
    }
}

Object *Object::parent() const
{
    return d->parent;
}

void Object::setParent(Object *parent)
{
    d->parent = parent;
}

int Object::childCount() const
{
    return 0;
}

void Object::resetData()
{

}

void Object::clearData()
{

}

ObjectPtr Object::copy() const
{
    ObjectPtr newObject = std::make_shared<Object>(*this);
    newObject->setParent(nullptr);
    return newObject;
}

ObjectPtr Object::clone() const
{
    ObjectPtr newObject = std::make_shared<Object>(*this);
    newObject->setParent(nullptr);
    return newObject;
}

Object &Object::operator =(const Object &other)
{
    if (this == &other) {
        return *this;
    }
    d->objectType  = other.d->objectType;
    d->id  = other.d->id;
    d->name = other.d->name;
    d->mark = other.d->mark;
    d->desc = other.d->desc;
    return *this;
}

Icd::ObjectPtr Object::findByDomain(const std::string &domain, int domainType,
                                    bool ignoreComplex) const
{
    (void)domain;
    (void)domainType;
    (void)ignoreComplex;
    return Icd::ObjectPtr();
}

bool Object::hasChildByName(const std::string &name, const ObjectPtr &exclude) const
{
    (void)name;
    (void)exclude;
    return false;
}

bool Object::hasChildByMark(const std::string &mark, const Icd::ObjectPtr &exclude) const
{
    (void)mark;
    (void)exclude;
    return false;
}

ObjectPtr Object::childAt(icd_uint64 index) const
{
    (void)index;
    return ObjectPtr();
}

ObjectPtr Object::replaceChild(icd_uint64 index, ObjectPtr &other)
{
    (void)index;
    (void)other;
    return ObjectPtr();
}

void Object::removeChild(icd_uint64 index)
{
    (void)index;
}

void Object::clearChildren()
{

}

Json::Value Object::save() const
{
    Json::Value json;
    // id
    if (!d->id.empty()) {
        switch (d->objectType) {
        case Icd::ObjectItem: break;
        default: json["id"] = id(); break;
        }
    }
    // name
    json["name"] = name();
    // mark
    if (!d->mark.empty()) {
        json["mark"] = mark();
    }
    // desc
    if (!d->desc.empty()) {
        json["desc"] = Json::toJson(desc());
    }
    return json;
}

bool Object::restore(const Json::Value &json, int)
{
    if (json.isMember("id")) {
        setId(json["id"].asString());
    }
    setName(json["name"].asString());
    setMark(json["mark"].asString());
    setDesc(Json::fromJson(json["desc"].asString()));
    return true;
}

} // ebnd of namespace Icd
