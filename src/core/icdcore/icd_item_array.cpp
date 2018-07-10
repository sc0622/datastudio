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
    case ArrayI8: return "i8";
    case ArrayU8: return "u8";
    case ArrayI16: return "i16";
    case ArrayU16: return "u16";
    case ArrayI32: return "i32";
    case ArrayU32: return "u32";
    case ArrayI64: return "i64";
    case ArrayU64: return "u64";
    case ArrayF32: return "f32";
    case ArrayF64: return "f64";
    default: return "?";
    }
}

ArrayType ArrayItem::stringArrayType(const std::string &str)
{
    typedef std::unordered_map<std::string, int> map_strtype;
    static const map_strtype::value_type map_data[ArrayTotal] = {
        map_strtype::value_type("i8", ArrayI8),
        map_strtype::value_type("u8", ArrayU8),
        map_strtype::value_type("i16", ArrayI16),
        map_strtype::value_type("u16", ArrayU16),
        map_strtype::value_type("i32", ArrayI32),
        map_strtype::value_type("u32", ArrayU32),
        map_strtype::value_type("i64", ArrayI64),
        map_strtype::value_type("u64", ArrayU64),
        map_strtype::value_type("f32", ArrayF32),
        map_strtype::value_type("f64", ArrayF64),
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

std::string ArrayItem::dataString() const
{
    return toString();
}

std::string ArrayItem::toString() const
{
    std::string s = std::string(buffer(), static_cast<size_t>(bufferSize()));
    std::string::size_type index = s.find_first_of('\0');
    if (index == std::string::npos) {
        return s;
    } else {
        return s.substr(0, index);
    }
}

int8_t *ArrayItem::i8() const
{
    return reinterpret_cast<int8_t*>(buffer());
}

uint8_t *ArrayItem::u8() const
{
    return reinterpret_cast<uint8_t*>(buffer());
}

int16_t *ArrayItem::i16() const
{
    return reinterpret_cast<int16_t*>(buffer());
}

uint16_t *ArrayItem::u16() const
{
    return reinterpret_cast<uint16_t*>(buffer());
}

int32_t *ArrayItem::i32() const
{
    return reinterpret_cast<int32_t*>(buffer());
}

uint32_t *ArrayItem::u32() const
{
    return reinterpret_cast<uint32_t*>(buffer());
}

int64_t *ArrayItem::i64() const
{
    return reinterpret_cast<int64_t*>(buffer());
}

uint64_t *ArrayItem::u64() const
{
    return reinterpret_cast<uint64_t*>(buffer());
}

float32_t *ArrayItem::f32() const
{
    return reinterpret_cast<float32_t*>(buffer());
}

float64_t *ArrayItem::f64() const
{
    return reinterpret_cast<float64_t*>(buffer());
}

double_t ArrayItem::valueOf(int index) const
{
    if (index < 0 || index >= d->count) {
        return 0.0;
    }

    switch (d->arrayType) {
    case Icd::ArrayI8: return static_cast<double>(i8()[index]);
    case Icd::ArrayU8: return static_cast<double>(u8()[index]);
    case Icd::ArrayI16: return static_cast<double>(i16()[index]);
    case Icd::ArrayU16: return static_cast<double>(u16()[index]);
    case Icd::ArrayI32: return static_cast<double>(i32()[index]);
    case Icd::ArrayU32: return static_cast<double>(u32()[index]);
    case Icd::ArrayI64: return static_cast<double>(i64()[index]);
    case Icd::ArrayU64: return static_cast<double>(u64()[index]);
    case Icd::ArrayF32: return static_cast<double>(f32()[index]);
    case Icd::ArrayF64: return static_cast<double>(f64()[index]);
    default: return 0.0;
    }
}

void ArrayItem::resetData()
{
    clearData();
}

void ArrayItem::clearData()
{
    char *_buffer = this->buffer();
    if (!_buffer) {
        return;
    }

    const int bufferSize = int(this->bufferSize());

    if (bufferSize > 0) {
        memset(_buffer, 0, bufferSize);
    }
}

void ArrayItem::setData(const char *buffer, int size)
{
    if (!buffer || size <= 0) {
        clearData();
        return;
    }

    char *_buffer = this->buffer();
    if (!_buffer) {
        return;
    }

    const int bufferSize = int(this->bufferSize());
    if (size > bufferSize) {
        size = bufferSize;
    } else {
        memset(_buffer + size, 0, bufferSize - size);
    }

    memcpy(_buffer, buffer, size_t(size));
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
