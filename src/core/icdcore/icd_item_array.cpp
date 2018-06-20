#include "precomp.h"
#include "icd_item_array.h"
#include <unordered_map>

namespace Icd {

// class ArrayItemData

class ArrayItemData
{
    friend class ArrayItem;
public:
    ArrayItemData()
        : arrayType(Int8Array)
        , count(0)
    {

    }

private:
    ArrayType arrayType;
    int count;
};

// class ArrayItem

ArrayItem::ArrayItem(Object *parent)
    : Item(ItemArray, parent)
    , d(new ArrayItemData())
{

}

ArrayItem::ArrayItem(const std::string &id, Object *parent)
    : Item(id, ItemArray, parent)
    , d(new ArrayItemData())
{

}

ArrayItem::ArrayItem(const ArrayItem &other)
    : Item(other)
    , d(new ArrayItemData())
{
    operator =(other);
}

ArrayItem::~ArrayItem()
{
    delete d;
}

ArrayType ArrayItem::arrayType() const
{
    return d->arrayType;
}

void ArrayItem::setArrayType(ArrayType type)
{
    d->arrayType = type;
    setBufferSize(d->count * typeSize());
}

std::string ArrayItem::arrayTypeString() const
{
    return arrayTypeString(d->arrayType);
}

std::string ArrayItem::arrayTypeString(ArrayType type)
{
    switch (type) {
    case Int8Array: return "i8a";
    case UInt8Array: return "u8a";
    case Int16Array: return "i16a";
    case UInt16Array: return "u16a";
    case Int32Array: return "i32a";
    case UInt32Array: return "u32a";
    case Int64Array: return "i64a";
    case UInt64Array: return "u64a";
    case Float32Array: return "f32a";
    case Float64Array: return "f64a";
    default: return "?";
    }
}

ArrayType ArrayItem::stringArrayType(const std::string &str)
{
    typedef std::unordered_map<std::string, int> map_strtype;
    static const map_strtype::value_type map_data[ArrayTypeTotal] = {
        map_strtype::value_type("i8a", Int8Array),
        map_strtype::value_type("u8a", UInt8Array),
        map_strtype::value_type("i16a", Int16Array),
        map_strtype::value_type("u16a", UInt16Array),
        map_strtype::value_type("i32a", Int32Array),
        map_strtype::value_type("u32a", UInt32Array),
        map_strtype::value_type("i64a", Int64Array),
        map_strtype::value_type("u64a", UInt64Array),
        map_strtype::value_type("f32a", Float32Array),
        map_strtype::value_type("f64a", Float64Array),
    };
    static const map_strtype _map(map_data, map_data + ArrayTypeTotal);
    map_strtype::const_iterator citer = _map.find(str);
    if (citer == _map.cend()) {
        return InvalidArray;
    } else {
        return ArrayType(citer->second);
    }
}

int ArrayItem::count() const
{
    return d->count;
}

void ArrayItem::setCount(int count)
{
    int _count = count;
    if (_count < 1) {
        _count = 1;
    }

    if (count != d->count) {
        d->count = count;
        setBufferSize(d->count * typeSize());
    }
}

std::string ArrayItem::typeName() const
{
    std::stringstream ss;
    ss << "icd_" << arrayTypeString();
    return ss.str();
}

std::string ArrayItem::typeString() const
{
    return Item::typeString() + "#" + arrayTypeString();
}

int ArrayItem::typeSize() const
{
    switch (d->arrayType) {
    case Int8Array:
    case UInt8Array: return 1;
    case Int16Array:
    case UInt16Array: return 2;
    case Int32Array:
    case UInt32Array: return 4;
    case Int64Array:
    case UInt64Array: return 8;
    case Float32Array: return 4;
    case Float64Array: return 8;
    default:
        break;
    }

    return 0;
}

Object *ArrayItem::clone() const
{
    return new ArrayItem(*this);
}

ArrayItem &ArrayItem::operator =(const ArrayItem &other)
{
    Item::operator =(other);
    d->arrayType = other.d->arrayType;
    d->count = other.d->count;
    return *this;
}

int8_t *ArrayItem::int8Array() const
{
    return reinterpret_cast<int8_t*>(buffer());
}

uint8_t *ArrayItem::uint8Array() const
{
    return reinterpret_cast<uint8_t*>(buffer());
}

int16_t *ArrayItem::int16Array() const
{
    return reinterpret_cast<int16_t*>(buffer());
}

uint16_t *ArrayItem::uint16Array() const
{
    return reinterpret_cast<uint16_t*>(buffer());
}

int32_t *ArrayItem::int32Array() const
{
    return reinterpret_cast<int32_t*>(buffer());
}

uint32_t *ArrayItem::uint32Array() const
{
    return reinterpret_cast<uint32_t*>(buffer());
}

int64_t *ArrayItem::int64Array() const
{
    return reinterpret_cast<int64_t*>(buffer());
}

uint64_t *ArrayItem::uint64Array() const
{
    return reinterpret_cast<uint64_t*>(buffer());
}

float_t *ArrayItem::floatArray() const
{
    return reinterpret_cast<float_t*>(buffer());
}

double_t *ArrayItem::doubleArray() const
{
    return reinterpret_cast<double_t*>(buffer());
}

Json::Value ArrayItem::save() const
{
    Json::Value json = Item::save();
    json["arrayType"] = arrayTypeString();
    json["count"] = d->count;
    return json;
}

bool ArrayItem::restore(const Json::Value &json, int deep)
{
    if (!Item::restore(json, deep)) {
        return false;
    }
    // arrayType
    ArrayType arrayType = stringArrayType(json["arrayType"].asString());
    if (arrayType == Icd::InvalidArray) {
        return false;
    }
    setArrayType(arrayType);
    // count
    setCount(json["count"].asInt());

    return true;
}

}
