#include "precomp.h"
#include "icd_item_numeric.h"
#include <sstream>
#include <unordered_map>

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
    NumericType numericType;    // ����������
    double scale;               // ������
    double decimals;            // С����Чλ
    double offset;              // ƫ��
    LimitItemPtr limit;         // ��Χ
    std::string unit;           // ��λ
    std::map<double, std::string> specs;  // ������,��ͼ����ʾʹ��
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
    case NumericI8:
    case NumericU8: setBufferSize(1); break;
    case NumericI16:
    case NumericU16: setBufferSize(2); break;
    case NumericI32:
    case NumericU32:
    case NumericF32: setBufferSize(4); break;
    case NumericI64:
    case NumericU64:
    case NumericF64: setBufferSize(8); break;
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
    case NumericI8: return "i8";
    case NumericU8: return "u8";
    case NumericI16: return "i16";
    case NumericU16: return "u16";
    case NumericI32: return "i32";
    case NumericU32: return "u32";
    case NumericI64: return "i64";
    case NumericU64: return "u64";
    case NumericF32: return "f32";
    case NumericF64: return "f64";
    default: return "?";
    }
}

NumericType NumericItem::stringNumericType(const std::string &str)
{
    typedef std::unordered_map<std::string, int> map_strtype;
    static const map_strtype::value_type map_data[NumericTotal] = {
        map_strtype::value_type("i8", NumericI8),
        map_strtype::value_type("u8", NumericU8),
        map_strtype::value_type("i16", NumericI16),
        map_strtype::value_type("u16", NumericU16),
        map_strtype::value_type("i32", NumericI32),
        map_strtype::value_type("u32", NumericU32),
        map_strtype::value_type("i64", NumericI64),
        map_strtype::value_type("u64", NumericU64),
        map_strtype::value_type("f32", NumericF32),
        map_strtype::value_type("f64", NumericF64)
    };
    static const map_strtype _map(map_data, map_data + NumericTotal);
    map_strtype::const_iterator citer = _map.find(str);
    if (citer == _map.cend()) {
        return NumericInvalid;
    } else {
        return NumericType(citer->second);
    }
}

double NumericItem::originalData() const
{
    return originalDataFromBuffer(buffer());
}

double NumericItem::data() const
{
    return dataFromBuffer(buffer());
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
    case NumericI8: *reinterpret_cast<icd_int8*>(buffer) = static_cast<icd_int8>(value); break;
    case NumericU8: *reinterpret_cast<icd_uint8*>(buffer)= static_cast<icd_uint8>(value); break;
    case NumericI16: *reinterpret_cast<icd_int16*>(buffer) = static_cast<icd_int16>(value); break;
    case NumericU16: *reinterpret_cast<icd_uint16*>(buffer) = static_cast<icd_uint16>(value); break;
    case NumericI32: *reinterpret_cast<icd_int32*>(buffer) = static_cast<icd_int32>(value); break;
    case NumericU32: *reinterpret_cast<icd_uint32*>(buffer) = static_cast<icd_uint32>(value); break;
    case NumericI64: *reinterpret_cast<icd_int64*>(buffer) = static_cast<icd_int64>(value); break;
    case NumericU64: *reinterpret_cast<icd_uint64*>(buffer) = static_cast<icd_uint64>(value); break;
    case NumericF32: *reinterpret_cast<icd_float32*>(buffer) = static_cast<icd_float32>(value); break;
    case NumericF64: *reinterpret_cast<icd_float64*>(buffer) = static_cast<icd_float64>(value); break;
    default: break;
    }
}

std::string NumericItem::dataString() const
{
    std::ostringstream os;
    os << data();
    return os.str();
}

double NumericItem::originalDataFromBuffer(const char *buffer) const
{
    if (!buffer) {
        return 0.0;
    }

    double value = 0.0;
    switch (d->numericType) {
    case NumericI8: value = *reinterpret_cast<const icd_int8*>(buffer); break;
    case NumericU8: value = *reinterpret_cast<const icd_uint8*>(buffer); break;
    case NumericI16: value = *reinterpret_cast<const icd_int16*>(buffer); break;
    case NumericU16: value = *reinterpret_cast<const icd_uint16*>(buffer); break;
    case NumericI32: value = *reinterpret_cast<const icd_int32*>(buffer); break;
    case NumericU32: value = *reinterpret_cast<const icd_uint32*>(buffer); break;
    case NumericI64: value = static_cast<double>(*reinterpret_cast<const icd_int64*>(buffer)); break;
    case NumericU64: value = static_cast<double>(*reinterpret_cast<const icd_uint64*>(buffer)); break;
    case NumericF32: value = static_cast<double>(*reinterpret_cast<const icd_float32*>(buffer)); break;
    case NumericF64: value = *reinterpret_cast<const icd_float64*>(buffer); break;
    default: return 0.0;
    }

    return value;
}

double NumericItem::dataFromBuffer(const char *buffer) const
{
    double value = originalDataFromBuffer(buffer) * d->scale + d->offset;
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

std::map<double, std::string> NumericItem::specs()
{
    return d->specs;
}

const std::map<double, std::string> &NumericItem::specs() const
{
    return d->specs;
}

void NumericItem::addSpec(double key, const std::string info)
{
    d->specs[key] = info;
}

std::string NumericItem::specAt(double key) const
{
    std::map<double, std::string>::const_iterator citer = d->specs.find(key);
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
            (-DBL_MAX, DBL_MAX);
    const int typeSize = (static_cast<int>(bufferSize()) << 3);

    if (d->limit->leftInf()) {
        switch (d->numericType) {
        case NumericU8:
        case NumericU16:
        case NumericU32:
        case NumericU64:
            range.first = 0.0;
            break;
        case NumericI8:
        case NumericI16:
        case NumericI32:
        case NumericF32:
        case NumericF64:
            range.first = double(-(1ll << (typeSize - 1)));
            break;
        default:
            break;
        }
    } else {
        range.first = (d->limit->minimum() - d->offset) / d->scale;
    }

    if (d->limit->rightInf()) {
        switch (d->numericType) {
        case NumericU8:
        case NumericU16:
        case NumericU32:
        case NumericU64:
            range.second = double((1ll << typeSize) - 1);
            break;
        case NumericI8:
        case NumericI16:
        case NumericI32:
        case NumericF32:
        case NumericF64:
            range.second = double((1ll << (typeSize - 1)) - 1);
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
    case NumericI8:
    case NumericI16:
    case NumericI32:
    case NumericI64: ss << "int"; break;
    case NumericU8:
    case NumericU16:
    case NumericU32:
    case NumericU64: ss << "uint"; break;
    case NumericF32:
    case NumericF64: ss << "float"; break;
    default: return "?";
    }
    ss << (static_cast<int>(bufferSize()) << 3);
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
        if (d->numericType == Icd::NumericF32) {
            float _value = float(value);
            float minimum = float(d->limit->minimum());
            if (!fuzzyCompare(_value, minimum) && _value < minimum) {
                return true;
            }
        } else {
            double minimum = d->limit->minimum();
            if (!fuzzyCompare(value, minimum) && value < minimum) {
                return true;
            }
        }
    }

    if (!d->limit->rightInf()) {
        if (d->numericType == Icd::NumericF32) {
            float _value = float(value);
            float maximum = float(d->limit->maximum());
            if (!fuzzyCompare(_value, maximum) && _value > maximum) {
                return true;
            }
        } else {
            double maximum = d->limit->maximum();
            if (!fuzzyCompare(value, maximum) && value > maximum) {
                return true;
            }
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
        for (std::map<double, std::string>::const_iterator
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
