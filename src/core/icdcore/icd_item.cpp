#include "precomp.h"
#include "icd_item.h"
#include "icdcore_inc.h"
#include <unordered_map>
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
        , buffer(0)
        , bufferSize(0.0)
        , bufferOffset(0.0)
        , defaultValue(0.0)
    {

    }

private:
    ItemType type;          // 数据项类型
    int itemOffset;         // 数据项偏移量（在所属表中的偏移量）
    char *buffer;           // 数据指针
    double bufferSize;      // 数据长度，因为BitMap和bitValue类型数据长度可能为小数，所以使用double类型
    double bufferOffset;    //
    double defaultValue;    // 默认值
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

ItemType Item::type() const
{
    return d->type;
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
    case ItemHead: return "head";
    case ItemCounter: return "counter";
    case ItemCheck: return "check";
    case ItemFrameCode: return "framecode";
    case ItemFrame: return "frame";
    case ItemString: return "string";
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
        map_strtype::value_type("head", ItemHead),
        map_strtype::value_type("counter", ItemCounter),
        map_strtype::value_type("check", ItemCheck),
        map_strtype::value_type("framecode", ItemFrameCode),
        map_strtype::value_type("frame", ItemFrame),
        map_strtype::value_type("string", ItemString),
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

Object *Item::clone() const
{
    return new Item(*this);
}

Item &Item::operator =(const Item &other)
{
    Object::operator =(other);
    d->type  = other.d->type;
    d->bufferSize = other.d->bufferSize;
    d->bufferOffset = other.d->bufferOffset;
    d->defaultValue = other.d->defaultValue;
    return *this;
}

ItemPtr Item::create(const std::string &id, ItemType type)
{
    if (id.empty()) {
        return ItemPtr();
    }

    switch (type) {
    case Icd::ItemNumeric: return Icd::ItemPtr(new Icd::NumericItem(id));
    case Icd::ItemBitMap:
    case Icd::ItemBitValue: return Icd::ItemPtr(new Icd::BitItem(id, type));
    case Icd::ItemComplex: return Icd::ItemPtr(new Icd::ComplexItem(id));
    case Icd::ItemHead: return Icd::ItemPtr(new Icd::HeaderItem(id));
    case Icd::ItemCounter: return Icd::ItemPtr(new Icd::CounterItem(id));
    case Icd::ItemCheck: return Icd::ItemPtr(new Icd::CheckItem(id));
    case Icd::ItemFrameCode: return Icd::ItemPtr(new Icd::FrameCodeItem(id));
    case Icd::ItemFrame: return Icd::ItemPtr(new Icd::FrameItem(id));
    case Icd::ItemString: return Icd::ItemPtr(new Icd::StringItem(id));
    case Icd::ItemDateTime: return Icd::ItemPtr(new Icd::DateTimeItem(id));
    case Icd::ItemArray: return Icd::ItemPtr(new Icd::ArrayItem(id));
    default:
        assert(false);
        return Icd::ItemPtr();  // not supported data type
    }
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
