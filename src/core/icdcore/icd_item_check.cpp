#include "precomp.h"
#include "icd_item_check.h"
#include <algorithm>
#include <sstream>
#include <unordered_map>
#include <assert.h>

namespace Icd {

class CheckItemData
{
    friend class CheckItem;
public:
    CheckItemData()
        : checkType(CheckNone)
        , startPos(0)
        , endPos(0)
    {

    }

private:
    CheckType checkType;
    int startPos;
    int endPos;
};

CheckItem::CheckItem(Object *parent)
    : Item(ItemCheck, parent)
    , d(new CheckItemData())
{

}

CheckItem::CheckItem(const std::string &id, Object *parent)
    : Item(id, ItemCheck, parent)
    , d(new CheckItemData())
{

}

CheckItem::CheckItem(const CheckItem &other)
    : Item(other)
    , d(new CheckItemData())
{
    operator =(other);
}

CheckItem::~CheckItem()
{
    delete d;
}

bool CheckItem::isValid() const
{
    return checkLength() > 0;
}

CheckType CheckItem::checkType() const
{
    return d->checkType;
}

void CheckItem::setCheckType(CheckType type)
{
    d->checkType = type;
    switch (type) {
    case CheckNone:
        setBufferSize(1);
        break;
    case CheckSum8:
    case CheckCrc8:
    case CheckXor8:
        setBufferSize(1);
        break;
    case CheckSum16:
    case CheckCrc16:
    case CheckXor16:
        setBufferSize(2);
        break;
    default:
        setBufferSize(0);
        break;
    }
}

std::string CheckItem::checkTypeString() const
{
    return checkTypeString(d->checkType);
}

std::string CheckItem::checkTypeString(CheckType type)
{
    switch (type) {
    case CheckSum8: return "sum8";
    case CheckSum16: return "sum16";
    case CheckCrc8: return "crc8";
    case CheckCrc16: return "crc16";
    case CheckXor8: return "xor8";
    case CheckXor16: return "xor16";
    case CheckNone: return "none";
    default: return "?";
    }
}

CheckType CheckItem::stringCheckType(const std::string &str)
{
    typedef std::unordered_map<std::string, int> map_strtype;
    static const map_strtype::value_type map_data[CheckTotal] = {
        map_strtype::value_type("sum8", CheckSum8),
        map_strtype::value_type("sum16", CheckSum16),
        map_strtype::value_type("crc8", CheckCrc8),
        map_strtype::value_type("crc16", CheckCrc16),
        map_strtype::value_type("xor8", CheckXor8),
        map_strtype::value_type("xor16", CheckXor16),
        map_strtype::value_type("none", CheckNone)
    };
    static const map_strtype _map(map_data, map_data + CheckTotal);
    map_strtype::const_iterator citer = _map.find(str);
    if (citer == _map.cend()) {
        return CheckInvalid;
    } else {
        return CheckType(citer->second);
    }
}

double CheckItem::data() const
{
    return dataFromBuffer(buffer());
}

void CheckItem::setData(double data)
{
    char *buffer = this->buffer();
    if (!buffer) {
        return;
    }

    unsigned short value = static_cast<unsigned short>(data);
    memcpy(buffer, &value, std::min<size_t>(sizeof(value), static_cast<size_t>(bufferSize())));
}

std::string CheckItem::dataString() const
{
    std::stringstream str;
    str << std::hex << static_cast<unsigned int>(data());
    return str.str();
}

double CheckItem::dataFromBuffer(const char *buffer) const
{
    if (!buffer) {
        return 0.0;
    }

    unsigned short value = 0;
    memcpy(&value, buffer, std::min<size_t>(sizeof(value), static_cast<size_t>(bufferSize())));
    return value;
}

int CheckItem::startPos() const
{
    return d->startPos;
}

void CheckItem::setStartPos(int startPos)
{
    if (startPos < 0) {
        d->startPos = 0;
    } else {
        d->startPos = startPos;
    }
}

int CheckItem::endPos() const
{
    return d->endPos;
}

void CheckItem::setEndPos(int endPos)
{
    if (endPos < 0) {
        d->endPos = 0;
    } else {
        d->endPos = endPos;
    }
}

int CheckItem::checkLength() const
{
    return d->endPos - d->startPos + 1;
}

std::string CheckItem::typeName() const
{
    std::stringstream ss;
    ss << "icd_uint" << (static_cast<int>(bufferSize())) * 8;
    return ss.str();
}

std::string CheckItem::typeString() const
{
    return Item::typeString() + "#" + checkTypeString();
}

ObjectPtr CheckItem::copy() const
{
    CheckItemPtr newCheck = std::make_shared<CheckItem>(*this);
    newCheck->setParent(nullptr);
    return newCheck;
}

ObjectPtr CheckItem::clone() const
{
    CheckItemPtr newCheck = std::make_shared<CheckItem>(*this);
    newCheck->setParent(nullptr);
    return newCheck;
}

CheckItem &CheckItem::operator =(const CheckItem &other)
{
    if (this == &other) {
        return *this;
    }
    Item::operator =(other);
    d->checkType = other.d->checkType;
    d->startPos = other.d->startPos;
    d->endPos = other.d->endPos;
    return *this;
}

Json::Value CheckItem::save() const
{
    Json::Value json = Item::save();

    json["checkType"] = checkTypeString();
    json["startPos"] = startPos();
    json["endPos"] = endPos();

    return json;
}

bool CheckItem::restore(const Json::Value &json, int deep)
{
    if (!Item::restore(json, deep)) {
        return false;
    }

    CheckType checkType = stringCheckType(json["checkType"].asString());
    if (checkType == Icd::CheckInvalid) {
        return false;
    }

    setCheckType(checkType);
    setStartPos(json["startPos"].asInt());
    setEndPos(json["endPos"].asInt());

    return true;
}

} // end of namespace Icd
