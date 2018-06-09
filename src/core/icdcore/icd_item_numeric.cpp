#include "precomp.h"
#include "icd_item_numeric.h"
#include <sstream>
#include <unordered_map>
#include <limits>

namespace Icd {

// class NumericItemData

class NumericItemData
{
    friend class NumericItem;
public:
    NumericItemData()
        : numericType(NumericInvalid)
        , scale(0.0)
        , decimals(0)
        , offset(0.0)
        , limit(new LimitItem())
    {

    }

private:
    NumericType numericType;    // 数据项类型
    double scale;               // 比例尺
    double decimals;            // 小数有效位
    double offset;              // 偏置
    LimitItemPtr limit;         // 范围
    std::string unit;           // 单位
    std::unordered_map<double, std::string> specs;  // 特征点,画图或显示使用
};

// class NumericItem

NumericItem::NumericItem(Object *parent)
    : Item(ItemNumeric, parent)
    , d(new NumericItemData())
{

}

NumericItem::NumericItem(const std::string &id, Object *parent)
    : Item(id, ItemNumeric, parent)
    , d(new NumericItemData())
{

}

NumericItem::NumericItem(const NumericItem &other)
    : Item(other)
    , d(new NumericItemData())
{
    operator =(other);
}

NumericItem::~NumericItem()
{
    delete d;
}

bool NumericItem::isValid() const
{
    return d->numericType != NumericInvalid;
}

NumericType NumericItem::numericType() const
{
    return d->numericType;
}

void NumericItem::setNumericType(Icd::NumericType type)
{
    d->numericType = type;
    switch (type) {
    case NumericInt8:
    case NumericUint8: setBufferSize(1); break;
    case NumericInt16:
    case NumericUint16: setBufferSize(2); break;
    case NumericInt32:
    case NumericUint32:
    case NumericFloat32: setBufferSize(4); break;
    case NumericInt64:
    case NumericUint64:
    case NumericFloat64: setBufferSize(8); break;
    default:
        break;
    }
}

std::string NumericItem::numericTypeString() const
{
    return numericTypeString(d->numericType);
}

std::string NumericItem::numericTypeString(Icd::NumericType type)
{
    switch (type) {
    case NumericInt8: return "i8";
    case NumericUint8: return "u8";
    case NumericInt16: return "i16";
    case NumericUint16: return "u16";
    case NumericInt32: return "i32";
    case NumericUint32: return "u32";
    case NumericInt64: return "i64";
    case NumericUint64: return "u64";
    case NumericFloat32: return "f32";
    case NumericFloat64: return "f64";
    default: return "?";
    }
}

NumericType NumericItem::stringNumericType(const std::string &str)
{
    typedef std::unordered_map<std::string, int> map_strtype;
    static const map_strtype::value_type map_data[NumericTotal] = {
        map_strtype::value_type("i8", NumericInt8),
        map_strtype::value_type("u8", NumericUint8),
        map_strtype::value_type("i16", NumericInt16),
        map_strtype::value_type("u16", NumericUint16),
        map_strtype::value_type("i32", NumericInt32),
        map_strtype::value_type("u32", NumericUint32),
        map_strtype::value_type("i64", NumericInt64),
        map_strtype::value_type("u64", NumericUint64),
        map_strtype::value_type("f32", NumericFloat32),
        map_strtype::value_type("f64", NumericFloat64)
    };
    static const map_strtype _map(map_data, map_data + _countof(map_data));
    map_strtype::const_iterator citer = _map.find(str);
    if (citer == _map.cend()) {
        return NumericInvalid;
    } else {
        return NumericType(citer->second);
    }
}

double NumericItem::originalData() const
{
    const char *buffer = this->buffer();
    if (!buffer) {
        return 0.0;
    }

    double value = 0.0;
    switch (d->numericType) {
    case NumericInt8: value = *(icd_int8 *)buffer; break;
    case NumericUint8: value = *(icd_uint8 *)buffer; break;
    case NumericInt16: value = *(icd_int16 *)buffer; break;
    case NumericUint16: value = *(icd_uint16 *)buffer; break;
    case NumericInt32: value = *(icd_int32 *)buffer; break;
    case NumericUint32: value = *(icd_uint32 *)buffer; break;
    case NumericInt64: value = double(*(icd_int64 *)buffer); break;
    case NumericUint64: value = double(*(icd_uint64 *)buffer); break;
    case NumericFloat32: value = *(icd_float32 *)buffer; break;
    case NumericFloat64: value = *(icd_float64 *)buffer; break;
    default: return 0.0;
    }

    return value;
}

double NumericItem::data() const
{
    double value = originalData() * d->scale + d->offset;
#if 0
    if (!d->limit->leftInf()) {
        if (value < d->limit->minimum()) {
            value = d->limit->minimum();
        }
    }

    if (!d->limit->rightInf()) {
        if (value > d->limit->maximum()) {
            value = d->limit->maximum();
        }
    }
#endif
    return value;
}

void NumericItem::setData(double data)
{
    char *buffer = this->buffer();
    if (!buffer) {
        return;
    }

    double value = data;
#if 0
    if (!d->limit->leftInf()) {
        if (value < d->limit->minimum()) {
            value = d->limit->minimum();
        }
    }

    if (!d->limit->rightInf()) {
        if (value > d->limit->maximum()) {
            value = d->limit->maximum();
        }
    }
#endif
    value = (value - d->offset) / d->scale;

    switch (d->numericType) {
    case NumericInt8: *(icd_int8 *)buffer = (icd_int8)value; break;
    case NumericUint8: *(icd_uint8 *)buffer = (icd_uint8)value; break;
    case NumericInt16: *(icd_int16 *)buffer = (icd_int16)value; break;
    case NumericUint16: *(icd_uint16 *)buffer = (icd_uint16)value; break;
    case NumericInt32: *(icd_int32 *)buffer = (icd_int32)value; break;
    case NumericUint32: *(icd_uint32 *)buffer = (icd_uint32)value; break;
    case NumericInt64: *(icd_int64 *)buffer = (icd_int64)value; break;
    case NumericUint64: *(icd_uint64 *)buffer = (icd_uint64)value; break;
    case NumericFloat32: *(icd_float32 *)buffer = (icd_float32)value; break;
    case NumericFloat64: *(icd_float64 *)buffer = (icd_float64)value; break;
    default: return;
    }
}

std::string NumericItem::dataString() const
{
    std::ostringstream os;
    os << data();
    return os.str();
}

double NumericItem::scale() const
{
    return d->scale;
}

void NumericItem::setScale(double scale)
{
    d->scale = scale;
}

double NumericItem::decimals() const
{
    return d->decimals;
}

void NumericItem::setDecimals(double value)
{
    d->decimals = value;
}

double NumericItem::offset() const
{
    return d->offset;
}

void NumericItem::setOffset(double offset)
{
    d->offset = offset;
}

LimitItemPtr NumericItem::limit() const
{
    return d->limit;
}

void NumericItem::setLimit(const LimitItemPtr &limit)
{
    d->limit = limit;
}

std::string NumericItem::unit() const
{
    return d->unit;
}

void NumericItem::setUnit(const std::string &unit)
{
    d->unit = unit;
}

std::unordered_map<double, std::string> NumericItem::specs()
{
    return d->specs;
}

const std::unordered_map<double, std::string> &NumericItem::specs() const
{
    return d->specs;
}

void NumericItem::addSpec(double key, const std::string info)
{
    d->specs[key] = info;
}

std::string NumericItem::specAt(double key) const
{
    std::unordered_map<double, std::string>::const_iterator citer = d->specs.find(key);
    if (citer != d->specs.cend()) {
        return citer->second;
    } else {
        return std::string();
    }
}

void NumericItem::removeSpec(double key)
{
    d->specs.erase(key);
}

void NumericItem::clearSpec()
{
    d->specs.clear();
}

std::pair<double, double> NumericItem::dataRange() const
{
    std::pair<double, double> range = std::make_pair<double, double>
            (-65535, 65535);
    const int typeSize = ((int)bufferSize() << 3);

    if (d->limit->leftInf()) {
        switch (d->numericType) {
        case NumericUint8:
        case NumericUint16:
        case NumericUint32:
        case NumericUint64:
            range.first = 0.0;
            break;
        case NumericInt8:
        case NumericInt16:
        case NumericInt32:
        case NumericFloat32:
        case NumericFloat64:
            range.first = double(-(0x1i64 << (typeSize - 1)));
            break;
        default:
            break;
        }
    } else {
        range.first = (d->limit->minimum() - d->offset) / d->scale;
    }

    if (d->limit->rightInf()) {
        switch (d->numericType) {
        case NumericUint8:
        case NumericUint16:
        case NumericUint32:
        case NumericUint64:
            range.second = double((0x1i64 << typeSize) - 1);
            break;
        case NumericInt8:
        case NumericInt16:
        case NumericInt32:
        case NumericFloat32:
        case NumericFloat64:
            range.second = double((0x1i64 << (typeSize - 1)) - 1);
            break;
        default:
            break;
        }
    } else {
        range.second = (d->limit->maximum() - d->offset) / d->scale;
    }

    return range;
}

std::pair<double, double> NumericItem::valueRange() const
{
    std::pair<double, double> range = dataRange();
    range.first = d->offset + range.first * d->scale;
    range.second = d->offset + range.second * d->scale;
    return range;
}

std::string NumericItem::typeName() const
{
    std::stringstream ss;
    ss << "icd_";
    switch (d->numericType) {
    case NumericInt8:
    case NumericInt16:
    case NumericInt32:
    case NumericInt64: ss << "int"; break;
    case NumericUint8:
    case NumericUint16:
    case NumericUint32:
    case NumericUint64: ss << "uint"; break;
    case NumericFloat32:
    case NumericFloat64: ss << "float"; break;
    default: return "?";
    }
    ss << ((int)bufferSize() << 3);
    return ss.str();
}

std::string NumericItem::typeString() const
{
    return numericTypeString();
}

bool NumericItem::outOfLimit() const
{
    double value = this->data();
    if (!d->limit->leftInf()) {
        if (value < d->limit->minimum()) {
            return true;
        }
    }

    if (!d->limit->rightInf()) {
        if (value > d->limit->maximum()) {
            return true;
        }
    }

    return false;
}

Object *NumericItem::clone() const
{
    return new NumericItem(*this);
}

NumericItem &NumericItem::operator =(const NumericItem &other)
{
    Item::operator =(other);
    d->numericType = other.d->numericType;
    d->scale = other.d->scale;
    d->decimals = other.d->decimals;
    d->offset = other.d->offset;
    d->limit = LimitItemPtr(other.d->limit->clone());
    d->unit = other.d->unit;
    d->specs = other.d->specs;
    return *this;
}

Json::Value NumericItem::save() const
{
    Json::Value json = Item::save();

    json["numericType"] = numericTypeString();
    json["scale"] = scale();
    json["offset"] = offset();
    json["decimals"] = decimals();
    Json::Value limitJson = d->limit->save();
    if (!limitJson.isNull()) {
        json["limit"] = limitJson;
    }
    if (!d->unit.empty()) {
        json["unit"] = d->unit;
    }
    if (!d->specs.empty()) {
        Json::Value specsJson;
        for (std::unordered_map<double, std::string>::const_iterator
             citer = d->specs.cbegin(); citer != d->specs.cend(); ++citer) {
            Json::Value specJson;
            specJson[Icd::dtoa(citer->first)] = citer->second;
            specsJson.append(specJson);
        }
        json["specs"] = specsJson;
    }

    return json;
}

bool NumericItem::restore(const Json::Value &json, int deep)
{
    if (!Item::restore(json, deep)) {
        return false;
    }

    setNumericType(stringNumericType(json["numericType"].asString()));
    setScale(json["scale"].asDouble());
    setOffset(json["offset"].asDouble());
    setDecimals(json["decimals"].asDouble());
    if (!d->limit->restore(json["limit"], deep)) {
        return false;
    }
    setUnit(json["unit"].asString());

    clearSpec();
    if (json.isMember("specs")) {
        Json::Value specsJson = json["specs"];
        Json::ArrayIndex specsSize = specsJson.size();
        for (Json::ArrayIndex i = 0; i < specsSize; ++i) {
            Json::Value specJson = specsJson[i];
            Json::ValueIterator specIter = specJson.begin();
            if (specIter != specJson.end()) {
                const std::string sKey = specIter.name();
                addSpec(Icd::atod(sKey), specJson[sKey].asString());
            }
        }
    }

    return true;
}

} // end of namespace Icd
