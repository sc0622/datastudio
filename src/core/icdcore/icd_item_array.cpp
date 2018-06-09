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
    {

    }

private:
    ArrayType arrayType;
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
    //TODO
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
    static const map_strtype _map(map_data, map_data + _countof(map_data));
    map_strtype::const_iterator citer = _map.find(str);
    if (citer == _map.cend()) {
        return InvalidArray;
    } else {
        return ArrayType(citer->second);
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
    std::stringstream ss;
    //TODO
    return ss.str();
}

Object *ArrayItem::clone() const
{
    return new ArrayItem(*this);
}

ArrayItem &ArrayItem::operator =(const ArrayItem &other)
{
    Item::operator =(other);
    d->arrayType = other.d->arrayType;
    return *this;
}

Json::Value ArrayItem::save() const
{
    Json::Value json = Item::save();
    json["arrayType"] = arrayTypeString();
    return json;
}

bool ArrayItem::restore(const Json::Value &json, int deep)
{
    if (!Item::restore(json, deep)) {
        return false;
    }

    ArrayType arrayType = stringArrayType(json["arrayType"].asString());
    if (arrayType == Icd::InvalidArray) {
        return false;
    }

    setArrayType(arrayType);
}

}
