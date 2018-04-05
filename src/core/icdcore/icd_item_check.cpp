#include "precomp.h"
#include "icd_item_check.h"
#include <algorithm>
#include <sstream>
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
    CheckType checkType;    // 校验类型
    int startPos;           // 校验起始位置
    int endPos;             // 校验终止位置
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
        assert(false);
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
    case CheckNone: return "none";
    case CheckSum8: return "sum8";
    case CheckSum16: return "sum16";
    case CheckCrc8: return "crc8";
    case CheckCrc16: return "crc16";
    case CheckXor8: return "xor8";
    case CheckXor16: return "xor16";
    default: return "?";
    }
}

CheckType CheckItem::stringCheckType(const std::string &str)
{
    typedef std::map<const std::string, const CheckType> map_strtype;
    static const map_strtype::value_type map_data[CheckTotal] = {
        map_strtype::value_type("none", CheckNone),
        map_strtype::value_type("sum8", CheckSum8),
        map_strtype::value_type("sum16", CheckSum16),
        map_strtype::value_type("crc8", CheckCrc8),
        map_strtype::value_type("crc16", CheckCrc16),
        map_strtype::value_type("xor8", CheckXor8),
        map_strtype::value_type("xor16", CheckXor16)
    };
    static const map_strtype _map(map_data, map_data + _countof(map_data));
    map_strtype::const_iterator citer = _map.find(str);
    if (citer == _map.cend()) {
        return CheckInvalid;
    } else {
        return citer->second;
    }
}

double CheckItem::data() const
{
    const char *buffer = this->buffer();
    if (!buffer) {
        return 0.0;
    }

    unsigned short value = 0;
    memcpy(&value, buffer, std::min<int>(sizeof(value), (int)bufferSize()));
    return value;
}

void CheckItem::setData(double data)
{
    char *buffer = this->buffer();
    if (!buffer) {
        return;
    }

    unsigned short value = (unsigned short)data;
    memcpy(buffer, &value, std::min<int>(sizeof(value), (int)bufferSize()));
}

std::string CheckItem::dataString() const
{
    std::stringstream str;
    str << std::hex << (unsigned int)data();
    return str.str();
}

int CheckItem::startPos() const
{
    return d->startPos;
}

void CheckItem::setStartPos(int startPos)
{
    if (startPos < 0) {
        assert(false);
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
    ss << "icd_uint" << ((int)bufferSize()) * 8;
    return ss.str();
}

std::string CheckItem::typeString() const
{
    return Item::typeString() + "#" + checkTypeString();
}

Object *CheckItem::clone() const
{
    return new CheckItem(*this);
}

CheckItem &CheckItem::operator =(const CheckItem &other)
{
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
        assert(false);
        return false;
    }
    setCheckType(checkType);
    setStartPos(json["startPos"].asInt());
    setEndPos(json["endPos"].asInt());

    return true;
}

} // end of namespace Icd
