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

bool Item::isSimpleItem() const
{
    return true;
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
    return newItem;
}

ObjectPtr Item::clone() const
{
    ItemPtr newItem = std::make_shared<Item>(*this);
    return newItem;
}

Item &Item::operator =(const Item &other)
{
    if (this == &other) {
        return *this;
    }
    Object::operator =(other);
    d->type  = other.d->type;
    d->itemOffset = other.d->itemOffset;
    d->buffer = other.d->buffer;
    d->bufferSize = other.d->bufferSize;
    d->bufferOffset = other.d->bufferOffset;
    d->defaultValue = other.d->defaultValue;
    return *this;
}

ItemPtr Item::create(const std::string &id, ItemType type, Object *parent)
{
    switch (type) {
    case Icd::ItemNumeric: return std::make_shared<NumericItem>(id, parent);
    case Icd::ItemBitMap:
    case Icd::ItemBitValue: return std::make_shared<BitItem>(id, type, parent);
    case Icd::ItemComplex: return std::make_shared<ComplexItem>(id, parent);
    case Icd::ItemHeader: return std::make_shared<HeaderItem>(id, parent);
    case Icd::ItemCounter: return std::make_shared<CounterItem>(id, parent);
    case Icd::ItemCheck: return std::make_shared<CheckItem>(id, parent);
    case Icd::ItemFrameCode: return std::make_shared<FrameCodeItem>(id, parent);
    case Icd::ItemFrame: return std::make_shared<FrameItem>(id, parent);
    case Icd::ItemDateTime: return std::make_shared<DateTimeItem>(id, parent);
    case Icd::ItemArray: return std::make_shared<ArrayItem>(id, parent);
    default:
        return Icd::ItemPtr();  // not supported data type
    }
}

ItemPtr Item::create(ItemType type, Object *parent)
{
    switch (type) {
    case Icd::ItemNumeric: return std::make_shared<NumericItem>(parent);
    case Icd::ItemBitMap:
    case Icd::ItemBitValue: return std::make_shared<BitItem>(type, parent);
    case Icd::ItemComplex: return std::make_shared<ComplexItem>(parent);
    case Icd::ItemHeader: return std::make_shared<HeaderItem>(parent);
    case Icd::ItemCounter: return std::make_shared<CounterItem>(parent);
    case Icd::ItemCheck: return std::make_shared<CheckItem>(parent);
    case Icd::ItemFrameCode: return std::make_shared<FrameCodeItem>(parent);
    case Icd::ItemFrame: return std::make_shared<FrameItem>(parent);
    case Icd::ItemDateTime: return std::make_shared<DateTimeItem>(parent);
    case Icd::ItemArray: return std::make_shared<ArrayItem>(parent);
    default:
        return Icd::ItemPtr();  // not supported data type
    }
}

ItemPtr Item::create(int index, const Json::Value &json, int deep, Object *parent)
{
    ItemType itemType = Item::stringType(json["type"].asString());

    Json::Value newJson;
    if (itemType == Icd::ItemComplex && json.isMember("size")
            && (json.size() == 0 || !json.isMember("table")
                || !json["table"].isMember("items"))) {
        itemType = Icd::ItemArray;
        newJson = json;
        newJson.removeMember("table");
        newJson["count"] = json["size"];
        newJson.removeMember("size");
        newJson["type"] = "array";
        newJson["arrayType"] = "i8";
    }

    Icd::ItemPtr newItem = Item::create(Icd::itoa(index), itemType, parent);
    if (!newItem) {
        return Icd::ItemPtr();
    }

    if (newJson.isNull()) {
        if (!newItem->restore(json, deep)) {
            return Icd::ItemPtr();
        }
    } else {
        if (!newItem->restore(newJson, deep)) {
            return Icd::ItemPtr();
        }
    }

    return newItem;
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

void Item::adjustBufferOffset()
{

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
