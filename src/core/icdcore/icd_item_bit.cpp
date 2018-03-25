﻿//#include "precomp.h"
#include "icd_item_bit.h"
#include <sstream>
#include <assert.h>

namespace Icd {

// class BitItemData

class BitItemData
{
    friend class BitItem;
public:
    BitItemData()
        : bitStart(0)
        , bitCount(0)
        , typeSize(0)
    {

    }

    static std::string &trim(std::string &str);

private:
    int bitStart;   // 起始比特位,从0开始
    int bitCount;   // 比特长度
    int typeSize;   // 数据类型字节数
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

// class BitItem

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

double BitItem::data() const
{
    //
    const char *buffer = this->buffer();
    if (!buffer) {
        return 0.0;
    }

    //
    int bitEnd = d->bitStart + d->bitCount;
    icd_uint64 value = 0;

    //
    if (bitEnd <= 8) {
        value = *(icd_uint8 *)buffer;
    } else if (bitEnd <= 16) {
        value = *(icd_uint16 *)buffer;
    } else if (bitEnd <= 32) {
        value = *(icd_uint32 *)buffer;
    } else if (bitEnd <= 64) {
        value = *(icd_uint64 *)buffer;
    } else {
        assert(false);
        // 不支持超过64位长度的bit类型
    }
    //
    value = (value << (64 - d->bitStart - d->bitCount)) >> (64 - d->bitCount);

    return (double)value;
}

void BitItem::setData(double data)
{
    //
    char *buffer = this->buffer();
    if (!buffer) {
        return;
    }

    //
    int bitEnd = d->bitStart + d->bitCount;
    icd_uint64 value = 0;

    //
    if (bitEnd <= 8) {
        value = *(icd_uint8 *)buffer;
    } else if (bitEnd <= 16) {
        value = *(icd_uint16 *)buffer;
    } else if (bitEnd <= 32) {
        value = *(icd_uint32 *)buffer;
    } else if (bitEnd <= 64) {
        value = *(icd_uint64 *)buffer;
    } else  {
        // 不支持超过32位长度的bit类型
    }

    //
    const icd_uint64 mask = (((1ui64 << d->bitCount) - 1) << d->bitStart);
    value = (value & (~mask)) | (((icd_uint64)data << d->bitStart) & mask);

    //
    if (bitEnd <= 8) {
        *(icd_uint8 *)buffer = (icd_uint8)value;
    } else if (bitEnd <= 16) {
        *(icd_uint16 *)buffer = (icd_uint16)value;
    } else if (bitEnd <= 32) {
        *(icd_uint32 *)buffer = (icd_uint32)value;
    } else if (bitEnd <= 64) {
        *(icd_uint64 *)buffer = (icd_uint64)value;
    } else {
        // 不支持超过64位长度的bit类型
    }
}

bool BitItem::testBit(int offset) const
{
    const uint32_t data = (uint32_t)this->data();
    return ((data & (1ui64 << offset)) != 0);
}

std::string BitItem::dataString() const
{
    unsigned int data = (unsigned int)this->data();
    switch (type()) {
    case ItemBitValue:
    {
        std::map<icd_uint64, std::string>::const_iterator citer = d->specs.find(data);
        if (citer != d->specs.end()) {
            return citer->second;
        } else {
            return "Invalid status";
        }
    }
    case ItemBitMap:
    {
        std::string str;
        std::map<icd_uint64,  std::string>::const_iterator citer = d->specs.begin();
        for (citer = d->specs.begin(); citer != d->specs.end(); ++citer) {
            if ((data >> citer->first) & 1ui64) {
                str.append(citer->second).append(" | ");
            }
        }
        return str.substr(0, str.size() - 3);
    }
    default:
        return "Invalid status";
    }
}

int BitItem::bitStart() const
{
    return d->bitStart;
}

void BitItem::setBitStart(int bitStart)
{
    if (bitStart < 0) {
        d->bitStart = 0;
    } else {
        d->bitStart = bitStart;
    }
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
        d->bitCount = count;
        setBufferSize(count / 8.0);
    }
}

int BitItem::typeSize() const
{
    return d->typeSize;
}

void BitItem::setTypeSize(int size)
{
    d->typeSize = size;
}

icd_uint64 BitItem::mask() const
{
    return (((1ui64 << d->bitCount) - 1) << d->bitStart);
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

void BitItem::remveSpec(icd_uint64 key)
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

std::string BitItem::nameAt(int offset) const
{
    const std::string spec = BitItem::nameOf(specAt(offset));
    switch (type()) {
    case Icd::ItemBitMap:
        return BitItem::nameOf(spec);
    case Icd::ItemBitValue:
    default:
        return spec;
    }

    return std::string();
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
        return BitItemData::trim(spec.substr(0, index));
    }

    return std::string();
}

std::string BitItem::descAt(int offset) const
{
    const std::string spec = specAt(offset);
    if (spec.empty()) {
        return std::string();
    }

    std::string::size_type index = spec.find_first_of(':');
    if (index == std::string::npos) {
        return std::string();
    }

    std::string descs = BitItemData::trim(spec.substr(index + 1));
    if (descs.empty()) {
        return std::string();
    }

    index = descs.find_first_of(';');

    if (testBit(offset)) {
        if (index == std::string::npos) {
            return std::string();
        } else {
            return BitItemData::trim(descs.substr(index + 1));
        }
    } else {
        if (index == std::string::npos) {
            return descs;
        } else {
            return BitItemData::trim(descs.substr(0, index));
        }
    }

    return std::string();
}

std::string BitItem::typeName() const
{
    std::ostringstream os;
    os << (typeSize() << 3);
    return "icd_uint" + os.str();
}

Object *BitItem::clone() const
{
    return new BitItem(*this);
}

BitItem &BitItem::operator =(const BitItem &other)
{
    Item::operator =(other);
    d->bitStart = other.d->bitStart;
    d->bitCount = other.d->bitCount;
    d->typeSize = other.d->typeSize;
    d->specs = other.d->specs;
    return *this;
}

Json::Value BitItem::save() const
{
    Json::Value json = Item::save();

    json["start"] = bitStart();
    json["count"] = bitCount();
    json["typeSize"] = typeSize();
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

    setBitStart(json["start"].asInt());
    setBitCount(json["count"].asInt());
    setTypeSize(json["typeSize"].asInt());

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
