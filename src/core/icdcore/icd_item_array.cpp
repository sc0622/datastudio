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
        : arrayType(ArrayI8)
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
    case ArrayI8: return "i8a";
    case ArrayU8: return "u8a";
    case ArrayI16: return "i16a";
    case ArrayU16: return "u16a";
    case ArrayI32: return "i32a";
    case ArrayU32: return "u32a";
    case ArrayI64: return "i64a";
    case ArrayU64: return "u64a";
    case ArrayF32: return "f32a";
    case ArrayF64: return "f64a";
    default: return "?";
    }
}

ArrayType ArrayItem::stringArrayType(const std::string &str)
{
    typedef std::unordered_map<std::string, int> map_strtype;
    static const map_strtype::value_type map_data[ArrayTotal] = {
        map_strtype::value_type("i8a", ArrayI8),
        map_strtype::value_type("u8a", ArrayU8),
        map_strtype::value_type("i16a", ArrayI16),
        map_strtype::value_type("u16a", ArrayU16),
        map_strtype::value_type("i32a", ArrayI32),
        map_strtype::value_type("u32a", ArrayU32),
        map_strtype::value_type("i64a", ArrayI64),
        map_strtype::value_type("u64a", ArrayU64),
        map_strtype::value_type("f32a", ArrayF32),
        map_strtype::value_type("f64a", ArrayF64),
    };
    static const map_strtype _map(map_data, map_data + ArrayTotal);
    map_strtype::const_iterator citer = _map.find(str);
    if (citer == _map.cend()) {
        return ArrayInvalid;
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
    case ArrayI8:
    case ArrayU8: return 1;
    case ArrayI16:
    case ArrayU16: return 2;
    case ArrayI32:
    case ArrayU32: return 4;
    case ArrayI64:
    case ArrayU64: return 8;
    case ArrayF32: return 4;
    case ArrayF64: return 8;
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
    if (arrayType == Icd::ArrayInvalid) {
        return false;
    }
    setArrayType(arrayType);
    // count
    setCount(json["count"].asInt());

    return true;
}

}
