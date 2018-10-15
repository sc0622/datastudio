#include "precomp.h"
#include "icd_item_bit.h"
#include <sstream>
#include <float.h>
#include <assert.h>

namespace Icd {

// class BitItemData

class BitItemData
{
    friend class BitItem;
public:
    BitItemData()
        : bitStart(0)
        , bitCount(8)
        , decimals(0)
        , offset(0.0)
        , scale(1.0)
        , limit(new LimitItem())
    {

    }

    static std::string &trim(std::string &str);
    static std::string trim(const std::string &str);

private:
    char bitStart;              // 起始比特位,从0开始
    char bitCount;              // 比特长度
    char decimals;              // 小数有效个数
    double offset;              // 偏置
    double scale;               // 比例尺
    LimitItemPtr limit;         // 范围
    std::string unit;           // 单位
    std::map<icd_uint64, std::string> specs;    // 特征点
};

std::string &BitItemData::trim(std::string &str)
{
    if (str.empty()) {
        return str;
    }
    str.erase(0, str.find_first_not_of(' '));
    str.erase(str.find_last_not_of(' ') + 1);
    return str;
}

std::string BitItemData::trim(const std::string &str)
{
    if (str.empty()) {
        return str;
    }
    std::string _str = str;
    _str.erase(0, _str.find_first_not_of(' '));
    _str.erase(_str.find_last_not_of(' ') + 1);
    return _str;
}

// class BitItem

BitItem::BitItem(Object *parent)
    : Item(Icd::ItemInvalid, parent)
    , d(new BitItemData())
{

}

BitItem::BitItem(ItemType type, Object *parent)
    : Item(type, parent)
    , d(new BitItemData())
{

}

BitItem::BitItem(const std::string &id, ItemType type, Object *parent)
    : Item(id, type, parent)
    , d(new BitItemData())
{

}

BitItem::BitItem(const BitItem &other)
    : Item(other)
    , d(new BitItemData())
{
    operator =(other);
}

BitItem::~BitItem()
{
    delete d;
}

int BitItem::rtti() const
{
    return type() == ItemBitMap ? ObjectBitMap : ObjectBitValue;
}

double BitItem::data() const
{
    return dataFromBuffer(buffer());
}

void BitItem::setData(double data)
{
    const int _bufferSize = bufferSize();
    if (_bufferSize <= 0) {
        return;
    }

    char *buffer = this->buffer();
    if (!buffer) {
        return;
    }

    icd_uint64 value = 0;

    memcpy(&value, buffer, size_t(_bufferSize));

    const icd_uint64 _mask = mask();
    value = (value & (~_mask)) | ((icd_uint64(data) << d->bitStart) & _mask);

    memcpy(buffer, &value, size_t(_bufferSize));
}

bool BitItem::testBit(int offset) const
{
    const uint32_t _data = uint32_t(data());
    return ((_data & (1ull << offset)) != 0);
}

std::string BitItem::dataString() const
{
    const unsigned int data = static_cast<unsigned int>(this->data());
    const ItemType itemType = type();
    if (itemType == ItemBitMap) {
        std::string str;
        std::map<icd_uint64,  std::string>::const_iterator citer = d->specs.begin();
        for (citer = d->specs.begin(); citer != d->specs.end(); ++citer) {
            if ((data >> citer->first) & 1ull) {
                str.append(citer->second).append(" | ");
            }
        }
        return str.substr(0, str.size() - 3);
    } else if (itemType == ItemBitValue) {
        std::map<icd_uint64, std::string>::const_iterator citer = d->specs.find(data);
        if (citer != d->specs.end()) {
            return citer->second;
        } else {
            return "Invalid status";
        }
    } else {
        return "Invalid status";
    }
}

double BitItem::dataFromBuffer(const char *buffer) const
{
    double value = originalDataFromBuffer(buffer);

    if (type() == Icd::ItemBitValue) {
        value = d->offset + value * d->scale;
    }

    return value;
}

bool BitItem::testBitFromBuffer(int offset, const char *buffer) const
{
    const uint32_t data = uint32_t(this->dataFromBuffer(buffer));
    return ((data & (1ull << offset)) != 0);
}

int BitItem::bitStart() const
{
    return d->bitStart;
}

void BitItem::setBitStart(int bitStart)
{
    if (bitStart < 0 || bitStart > 63) {
        d->bitStart = 0;
    } else {
        d->bitStart = char(bitStart);
    }

    setBufferSize(calcSize());
}

int BitItem::bitCount() const
{
    return d->bitCount;
}

void BitItem::setBitCount(int count)
{
    if (count < 0 || count > 64) {
        d->bitCount = 0;
        setBufferSize(0);
    } else {
        d->bitCount = char(count);
        setBufferSize(calcSize());
    }
}

int BitItem::calcSize() const
{
    const char bitEnd = d->bitStart + d->bitCount;
    if (bitEnd <= 0) {
        return 0;
    } else if (bitEnd <= 8) {
        return 1;
    } else if (bitEnd <= 16) {
        return 2;
    } else if (bitEnd <= 32) {
        return 4;
    } else if (bitEnd <= 64) {
        return 8;
    } else {
        return 8;   //
    }
}

icd_uint64 BitItem::mask() const
{
    return (((1ull << d->bitCount) - 1) << d->bitStart);
}

std::map<icd_uint64, std::string> BitItem::specs()
{
    return d->specs;
}

const std::map<icd_uint64, std::string> &BitItem::specs() const
{
    return d->specs;
}

void BitItem::addSpec(icd_uint64 key, const std::string &info)
{
    d->specs[key] = info;
}

void BitItem::removeSpec(icd_uint64 key)
{
    d->specs.erase(key);
}

void BitItem::clearSpec()
{
    d->specs.clear();
}

std::string BitItem::specAt(icd_uint64 key) const
{
    std::map<icd_uint64, std::string>::const_iterator citer = d->specs.find(key);
    if (citer != d->specs.cend()) {
        return citer->second;
    } else {
        return std::string();
    }
}

icd_uint64 BitItem::keyOf(const std::string &info, bool *ok)
{
    std::map<icd_uint64, std::string>::const_iterator citer = d->specs.cbegin();
    for (; citer != d->specs.cend(); ++citer) {
        if (citer->second == info) {
            if (ok) {
                *ok = true;
            }
            return citer->first;
        }
    }

    if (ok) {
        *ok = false;
    }

    return icd_uint64(-1);
}

std::string BitItem::nameAt(int offset) const
{
    const std::string spec = BitItem::nameOf(specAt(icd_uint64(offset)));
    if (type() == Icd::ItemBitMap) {
        return nameOf(spec);
    } else {
        return spec;
    }
}

std::string BitItem::nameOf(const std::string &spec)
{
    if (spec.empty()) {
        return std::string();
    }

    std::string::size_type index = spec.find_first_of(':');
    if (index == std::string::npos) {
        return spec;
    } else {
        std::string _str = spec.substr(0, index);
        return BitItemData::trim(_str);
    }
}

std::string BitItem::descAt(int offset) const
{
    const std::string spec = specAt(icd_uint64(offset));
    if (spec.empty()) {
        return std::string();
    }

    std::string::size_type index = spec.find_first_of(':');
    if (index == std::string::npos) {
        return std::string();
    }

    std::string _section = spec.substr(index + 1);
    std::string descs = BitItemData::trim(_section);
    if (descs.empty()) {
        return std::string();
    }

    index = descs.find_first_of(';');

    if (testBit(offset)) {
        if (index == std::string::npos) {
            return std::string();
        } else {
            _section = descs.substr(index + 1);
            return BitItemData::trim(_section);
        }
    } else {
        if (index == std::string::npos) {
            return descs;
        } else {
            _section = descs.substr(0, index);
            return BitItemData::trim(_section);
        }
    }
}

std::string BitItem::typeName() const
{
    std::ostringstream os;
    os << (bufferSize() << 3);
    return "icd_uint" + os.str();
}

ObjectPtr BitItem::copy() const
{
    BitItemPtr newBit = std::make_shared<BitItem>(*this);
    return newBit;
}

ObjectPtr BitItem::clone() const
{
    BitItemPtr newBit = std::make_shared<BitItem>(*this);
    // children
    newBit->setLimit(JHandlePtrCast<LimitItem>(d->limit->clone()));
    return newBit;
}

BitItem &BitItem::operator =(const BitItem &other)
{
    if (this == &other) {
        return *this;
    }
    Item::operator =(other);
    d->bitStart = other.d->bitStart;
    d->bitCount = other.d->bitCount;
    d->offset = other.d->offset;
    d->scale = other.d->scale;
    d->decimals = other.d->decimals;
    d->unit = other.d->unit;
    d->specs = other.d->specs;
    return *this;
}

double BitItem::originalData() const
{
    return originalDataFromBuffer(buffer());
}

double BitItem::originalDataFromBuffer(const char *buffer) const
{
    const int _bufferSize = bufferSize();
    if (_bufferSize <= 0) {
        return 0.0;
    }

    if (!buffer) {
        return 0.0;
    }

    icd_uint64 value = 0;
    memcpy(&value, buffer, size_t(_bufferSize));

    value = (value << (64 - d->bitStart - d->bitCount)) >> (64 - d->bitCount);

    return double(value);
}

double BitItem::offset() const
{
    return d->offset;
}

void BitItem::setOffset(double offset)
{
    d->offset = offset;
}

double BitItem::scale() const
{
    return d->scale;
}

void BitItem::setScale(double scale)
{
    d->scale = scale;
}

int BitItem::decimals() const
{
    return d->decimals;
}

void BitItem::setDecimals(int value)
{
    d->decimals = char(value);
}

LimitItemPtr BitItem::limit() const
{
    return d->limit;
}

void BitItem::setLimit(const LimitItemPtr &limit)
{
    d->limit = limit;
}

std::string BitItem::unit() const
{
    return d->unit;
}

void BitItem::setUnit(const std::string &unit)
{
    d->unit = unit;
}

std::pair<double, double> BitItem::dataRange() const
{
    std::pair<double, double> range = std::make_pair<double, double>
            (0, (1UL << d->bitCount));

    if (d->limit->minimumInf()) {
        range.first = 0;
    } else {
        range.first = (d->limit->maximum() - d->offset) / d->scale;
    }

    if (d->limit->maximumInf()) {
        range.second = (1UL << d->bitCount);
    } else {
        range.second = (d->limit->minimum() - d->offset) / d->scale;
    }

    return range;
}

std::pair<double, double> BitItem::valueRange() const
{
    std::pair<double, double> range = dataRange();
    range.first = d->offset + range.first * d->scale;
    range.second = d->offset + range.second * d->scale;
    return range;
}

bool BitItem::outOfLimit() const
{
    double value = this->data();
    if (!d->limit->minimumInf()) {
        double minimum = d->limit->minimum();
        if (!fuzzyCompare(value, minimum) && value < minimum) {
            return true;
        }
    }

    if (!d->limit->maximumInf()) {
        double maximum = d->limit->maximum();
        if (!fuzzyCompare(value, maximum) && value > maximum) {
            return true;
        }
    }

    return false;
}

std::string BitItem::prettyValue() const
{
    return prettyValue(data());
}

std::string BitItem::prettyValue(double value)
{
    std::ostringstream oss;
    oss.precision(DBL_DIG);
    oss << std::uppercase << value;
    std::string str = oss.str();
    return str;
}

Json::Value BitItem::save() const
{
    Json::Value json = Item::save();

    json["start"] = d->bitStart;
    json["count"] = d->bitCount;
    json["offset"] = d->offset;
    json["scale"] = d->scale;
    // decimals
    if (d->decimals > 0) {
        json["decimals"] = d->decimals;
    }
    // limit
    Json::Value limitJson = d->limit->save();
    if (!limitJson.isNull()) {
        json["limit"] = limitJson;
    }
    if (!d->unit.empty()) {
        json["unit"] = d->unit;
    }
    // specs
    if (!d->specs.empty()) {
        Json::Value specsJson;
        for (std::map<icd_uint64, std::string>::const_iterator
             citer = d->specs.cbegin(); citer != d->specs.cend(); ++citer) {
            Json::Value specJson;
            specJson[Icd::u64toa(citer->first, true)] = citer->second;
            specsJson.append(specJson);
        }
        json["specs"] = specsJson;
    }

    return json;
}

bool BitItem::restore(const Json::Value &json, int deep)
{
    if (!Item::restore(json, deep)) {
        return false;
    }

    // bitstart
    setBitStart(json["start"].asInt());
    // bitcount
    setBitCount(json["count"].asInt());
    // offset
    setOffset(json["offset"].asDouble());
    // scale
    if (json.isMember("scale")) {
        setScale(json["scale"].asDouble());
    }
    // decimals
    setDecimals(json["decimals"].asInt());
    // limit
    if (!d->limit->restore(json["limit"], deep)) {
        return false;
    }
    // unit
    setUnit(json["unit"].asString());
    // specs
    clearSpec();
    if (json.isMember("specs")) {
        Json::Value specsJson = json["specs"];
        Json::ArrayIndex specsSize = specsJson.size();
        for (Json::ArrayIndex i = 0; i < specsSize; ++i) {
            Json::Value specJson = specsJson[i];
            Json::ValueIterator specIter = specJson.begin();
            if (specIter != specJson.end()) {
                const std::string sKey = specIter.name();
                addSpec(Icd::atou64(sKey), specJson[sKey].asString());
            }
        }
    }

    return true;
}

} // end of namespace Icd
