#include "precomp.h"
#include "icd_item.h"
#include "icdcore_inc.h"
#include <unordered_map>
#include <limits>
#include <assert.h>

namespace Icd {

// class ItemData

class ItemData
{
    friend class Item;
public:
    ItemData()
        : type(ItemInvalid)
        , itemOffset(1)
        , buffer(nullptr)
        , bufferSize(0.0)
        , bufferOffset(0.0)
        , defaultValue(0.0)
    {

    }

private:
    ItemType type;
    int itemOffset;
    char *buffer;
    double bufferSize;
    double bufferOffset;
    double defaultValue;
};

// class Item

Item::Item(ItemType type, Object *parent)
    : Object(ObjectItem, parent)
    , d(new ItemData())
{
    d->type = type;
}

Item::Item(const std::string &id, ItemType type, Object *parent)
    : Object(id, ObjectItem, parent)
    , d(new ItemData())
{
    d->type = type;
}

Item::Item(const Item &other)
    : Object(other)
    , d(new ItemData())
{
    operator =(other);
}

Item::~Item()
{
    delete d;
}

int Item::rtti() const
{
    return ObjectItem;
}

ItemType Item::type() const
{
    return static_cast<ItemType>(d->type);
}

int Item::itemOffset() const
{
    return d->itemOffset;
}

char *Item::buffer() const
{
    return d->buffer;
}

void Item::setBuffer(char *buffer)
{
    d->buffer = buffer;
}

double Item::bufferSize() const
{
    return d->bufferSize;
}

double Item::bufferOffset() const
{
    return d->bufferOffset;
}

double Item::localOffset() const
{
    Object *parent = this->parent();
    if (!parent) {
        return bufferOffset();
    }

    if (parent->objectType() != Icd::ObjectTable) {
        return bufferOffset();
    }

    const Icd::Table *table = dynamic_cast<Icd::Table *>(parent);
    if (!table || !table->parent()) {
        return bufferOffset();
    }

    return bufferOffset() - table->bufferOffset();
}

double Item::defaultValue() const
{
    return d->defaultValue;
}

void Item::setDefaultValue(double value)
{
    d->defaultValue = value;
    setData(value);
}

double Item::dataFromBuffer(const char *buffer) const
{
    (void)buffer;
    return 0.0;
}

std::string Item::typeName() const
{
    return typeString();
}

std::string Item::codeName() const
{
    return mark();
}

std::string Item::typeString() const
{
    return typeString(d->type);
}

std::string Item::typeString(ItemType type)
{
    switch (type) {
    case ItemNumeric: return "numeric";
    case ItemBitMap: return "bitmap";
    case ItemBitValue: return "bitvalue";
    case ItemComplex: return "complex";
    case ItemHeader: return "head";
    case ItemCounter: return "counter";
    case ItemCheck: return "check";
    case ItemFrameCode: return "framecode";
    case ItemFrame: return "frame";
    case ItemDateTime: return "datetime";
    case ItemArray: return "array";
    default: return "invalid";
    }
}

ItemType Item::stringType(const std::string &str)
{
    typedef std::unordered_map<std::string, int> map_strtype;
    static const map_strtype::value_type map_data[ItemTotal] = {
        map_strtype::value_type("numeric", ItemNumeric),
        map_strtype::value_type("bitmap", ItemBitMap),
        map_strtype::value_type("bitvalue", ItemBitValue),
        map_strtype::value_type("complex", ItemComplex),
        map_strtype::value_type("head", ItemHeader),
        map_strtype::value_type("counter", ItemCounter),
        map_strtype::value_type("check", ItemCheck),
        map_strtype::value_type("framecode", ItemFrameCode),
        map_strtype::value_type("frame", ItemFrame),
        map_strtype::value_type("datetime", ItemDateTime),
        map_strtype::value_type("array", ItemArray)
    };
    static const map_strtype _map(map_data, map_data + ItemTotal);
    map_strtype::const_iterator citer = _map.find(str);
    if (citer == _map.cend()) {
        return ItemInvalid;
    } else {
        return ItemType(citer->second);
    }
}

void Item::resetData()
{
    setData(defaultValue());
}

void Item::clearData()
{
    setData(0);
}

ObjectPtr Item::copy() const
{
    ItemPtr newItem = std::make_shared<Item>(*this);
    newItem->setParent(nullptr);
    return newItem;
}

ObjectPtr Item::clone() const
{
    ItemPtr newItem = std::make_shared<Item>(*this);
    newItem->setParent(nullptr);
    return newItem;
}

Item &Item::operator =(const Item &other)
{
    if (this == &other) {
        return *this;
    }
    Object::operator =(other);
    d->type  = other.d->type;
    d->bufferSize = other.d->bufferSize;
    d->bufferOffset = other.d->bufferOffset;
    d->defaultValue = other.d->defaultValue;
    return *this;
}

ItemPtr Item::create(const std::string &id, ItemType type)
{
    switch (type) {
    case Icd::ItemNumeric: return Icd::ItemPtr(new Icd::NumericItem(id));
    case Icd::ItemBitMap:
    case Icd::ItemBitValue: return Icd::ItemPtr(new Icd::BitItem(id, type));
    case Icd::ItemComplex: return Icd::ItemPtr(new Icd::ComplexItem(id));
    case Icd::ItemHeader: return Icd::ItemPtr(new Icd::HeaderItem(id));
    case Icd::ItemCounter: return Icd::ItemPtr(new Icd::CounterItem(id));
    case Icd::ItemCheck: return Icd::ItemPtr(new Icd::CheckItem(id));
    case Icd::ItemFrameCode: return Icd::ItemPtr(new Icd::FrameCodeItem(id));
    case Icd::ItemFrame: return Icd::ItemPtr(new Icd::FrameItem(id));
    case Icd::ItemDateTime: return Icd::ItemPtr(new Icd::DateTimeItem(id));
    case Icd::ItemArray: return Icd::ItemPtr(new Icd::ArrayItem(id));
    default:
        return Icd::ItemPtr();  // not supported data type
    }
}

ItemPtr Item::create(ItemType type)
{
    switch (type) {
    case Icd::ItemNumeric: return Icd::ItemPtr(new Icd::NumericItem());
    case Icd::ItemBitMap:
    case Icd::ItemBitValue: return Icd::ItemPtr(new Icd::BitItem(type));
    case Icd::ItemComplex: return Icd::ItemPtr(new Icd::ComplexItem());
    case Icd::ItemHeader: return Icd::ItemPtr(new Icd::HeaderItem());
    case Icd::ItemCounter: return Icd::ItemPtr(new Icd::CounterItem());
    case Icd::ItemCheck: return Icd::ItemPtr(new Icd::CheckItem());
    case Icd::ItemFrameCode: return Icd::ItemPtr(new Icd::FrameCodeItem());
    case Icd::ItemFrame: return Icd::ItemPtr(new Icd::FrameItem());
    case Icd::ItemDateTime: return Icd::ItemPtr(new Icd::DateTimeItem());
    case Icd::ItemArray: return Icd::ItemPtr(new Icd::ArrayItem());
    default:
        return Icd::ItemPtr();  // not supported data type
    }
}

bool Item::isSubFrameItem() const
{
    Icd::Object *object = parent();
    while (object) {
        const Icd::ObjectType objectType = object->objectType();
        if (objectType < Icd::ObjectTable) {
            break;
        }

        if (objectType == Icd::ObjectTable) {
            const Icd::Table *table = dynamic_cast<Icd::Table*>(object);
            if (!table) {
                break;
            }
            object = object->parent();
            if (object && object->objectType() == Icd::ObjectItem) {
                Icd::Item *item = dynamic_cast<Icd::Item*>(object);
                if (!item) {
                    break;
                }
                if (item->type() == Icd::ItemFrame) {
                    return true;
                }
            }
            continue;
        }

        object = object->parent();
    }

    return false;
}

Icd::Table *Item::subFrameTable() const
{
    Icd::Object *object = parent();
    while (object) {
        const Icd::ObjectType objectType = object->objectType();
        if (objectType < Icd::ObjectTable) {
            break;
        }

        if (objectType == Icd::ObjectTable) {
            Icd::Table *table = dynamic_cast<Icd::Table*>(object);
            if (!table) {
                break;
            }
            object = object->parent();
            if (object && object->objectType() == Icd::ObjectItem) {
                Icd::Item *item = dynamic_cast<Icd::Item*>(object);
                if (!item) {
                    break;
                }
                if (item->type() == Icd::ItemFrame) {
                    return table;
                }
            }
            continue;
        }

        object = object->parent();
    }

    return nullptr;
}

bool Item::fuzzyCompare(double p1, double p2)
{
    return std::abs(p1 - p2) * 1000000000000. <= std::min(std::abs(p1), std::abs(p2));
}

bool Item::fuzzyCompare(float p1, float p2)
{
    return std::fabs(p1 - p2) * 100000.f <= std::min(std::fabs(p1), std::fabs(p2));
}

void Item::setType(ItemType type)
{
    d->type = type;
}

void Item::setBufferSize(double size)
{
    d->bufferSize = size;
}

void Item::setItemOffset(int offset)
{
    d->itemOffset = offset;
}

void Item::setBufferOffset(double offset)
{
    d->bufferOffset = offset;
}

Json::Value Item::save() const
{
    Json::Value json = Object::save();
    json["type"] = Item::typeString();
    json["defaultValue"] = Item::defaultValue();
    return json;
}

bool Item::restore(const Json::Value &json, int deep)
{
    if (!Object::restore(json, deep)) {
        return false;
    }
    Item::setType(stringType(json["type"].asString()));
    Item::setDefaultValue(json["defaultValue"].asDouble());
    return true;
}

} // end of namespace Icd
