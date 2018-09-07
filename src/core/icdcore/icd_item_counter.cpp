#include "precomp.h"
#include "icd_item_counter.h"
#include <algorithm>
#include <sstream>
#include <unordered_map>
#include <assert.h>

namespace Icd {

class CounterItemData
{
    friend class CounterItem;
public:
    CounterItemData()
        : counterType(CounterU8)
    {

    }

private:
    CounterType counterType;
};

CounterItem::CounterItem(Object *parent)
    : Item(ItemCounter, parent)
    , d(new CounterItemData())
{

}

CounterItem::CounterItem(const std::string &id, Object *parent)
    : Item(id, ItemCounter, parent)
    , d(new CounterItemData())
{

}

CounterItem::CounterItem(const CounterItem &other)
    : Item(other)
    , d(new CounterItemData())
{
    operator =(other);
}

CounterItem::~CounterItem()
{
    delete d;
}

CounterType CounterItem::counterType() const
{
    return d->counterType;
}

void CounterItem::setCounterType(CounterType type)
{
    d->counterType = type;
    switch (type) {
    case CounterU8: setBufferSize(1); break;
    case CounterU16: setBufferSize(2); break;
    case CounterU32: setBufferSize(4); break;
    case CounterU64: setBufferSize(8); break;
    default: break;
    }
}

std::string CounterItem::counterTypeString() const
{
    return counterTypeString(d->counterType);
}

std::string CounterItem::counterTypeString(CounterType type)
{
    switch (type) {
    case CounterU8: return "u8";
    case CounterU16: return "u16";
    case CounterU32: return "u32";
    case CounterU64: return "u64";
    default: return "?";
    }
}

CounterType CounterItem::stringCounterType(const std::string &str)
{
    typedef std::unordered_map<std::string, int> map_strtype;
    static const map_strtype::value_type map_data[CounterTotal] = {
        map_strtype::value_type("u8", CounterU8),
        map_strtype::value_type("u16", CounterU16),
        map_strtype::value_type("u32", CounterU32),
        map_strtype::value_type("u64", CounterU64)
    };
    static const map_strtype _map(map_data, map_data + CounterTotal);
    map_strtype::const_iterator citer = _map.find(str);
    if (citer == _map.cend()) {
        return CounterInvalid;
    } else {
        return CounterType(citer->second);
    }
}

double CounterItem::data() const
{
    return dataFromBuffer(buffer());
}

void CounterItem::setData(double data)
{
    char *buffer = this->buffer();
    if (!buffer) {
        return;
    }

    unsigned int value = static_cast<unsigned int>(data);
    memcpy(buffer, &value, std::min<size_t>(sizeof(value), static_cast<size_t>(bufferSize())));
}

std::string CounterItem::dataString() const
{
    std::stringstream str;
    str << data();
    return str.str();
}

unsigned char CounterItem::value() const
{
    return 0;
}

void CounterItem::setValue(unsigned char value)
{
    (void)value;
}

double CounterItem::dataFromBuffer(const char *buffer) const
{
    if (!buffer) {
        return 0.0;
    }

    unsigned int value = 0;
    memcpy(&value, buffer, std::min<size_t>(sizeof(value), static_cast<size_t>(bufferSize())));
    return value;
}

std::string CounterItem::typeName() const
{
    std::stringstream ss;
    ss << "icd_uint" << (static_cast<int>(bufferSize())) * 8;
    return ss.str();
}

std::string CounterItem::typeString() const
{
    return Item::typeString() + "#" + counterTypeString();
}

ObjectPtr CounterItem::copy() const
{
    CounterItemPtr newCounter = std::make_shared<CounterItem>(*this);
    newCounter->setParent(nullptr);
    return newCounter;
}

ObjectPtr CounterItem::clone() const
{
    CounterItemPtr newCounter = std::make_shared<CounterItem>(*this);
    newCounter->setParent(nullptr);
    return newCounter;
}

CounterItem &CounterItem::operator =(const CounterItem &other)
{
    if (this == &other) {
        return *this;
    }
    Item::operator =(other);
    d->counterType = other.d->counterType;
    return *this;
}

Json::Value CounterItem::save() const
{
    Json::Value json = Item::save();
    json["counterType"] = counterTypeString();
    return json;
}

bool CounterItem::restore(const Json::Value &json, int deep)
{
    if (!Item::restore(json, deep)) {
        return false;
    }

    CounterType counterType = stringCounterType(json["counterType"].asString());
    if (counterType == Icd::CounterInvalid) {
        return false;
    }

    setCounterType(counterType);

    return true;
}

} // end of namespace Icd
