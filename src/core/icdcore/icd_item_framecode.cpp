#include "precomp.h"
#include "icd_item_framecode.h"
#include "icd_table.h"
#include <algorithm>
#include <sstream>
#include <unordered_map>
#include <assert.h>

namespace Icd {

class FrameCodeItemData
{
    friend class FrameCodeItem;
public:
    FrameCodeItemData()
        : frameCodeType(FrameCodeInvalid)
    {

    }

private:
    FrameCodeType frameCodeType;
    std::string frameId;
    FrameItemPtr frame;
};

FrameCodeItem::FrameCodeItem(Object *parent)
    : Item(ItemFrameCode, parent)
    , d(new FrameCodeItemData())
{

}

FrameCodeItem::FrameCodeItem(const std::string &id, Object *parent)
    : Item(id, ItemFrameCode, parent)
    , d(new FrameCodeItemData())
{

}

FrameCodeItem::FrameCodeItem(const FrameCodeItem &other)
    : Item(other)
    , d(new FrameCodeItemData())
{
    operator =(other);
}

FrameCodeItem::~FrameCodeItem()
{
    delete d;
}

FrameCodeType FrameCodeItem::frameCodeType() const
{
    return d->frameCodeType;
}

void FrameCodeItem::setFrameCodeType(FrameCodeType type)
{
    d->frameCodeType = type;
    switch (type) {
    case FrameCodeU8: setBufferSize(1); break;
    case FrameCodeU16: setBufferSize(2); break;
    case FrameCodeU32: setBufferSize(4); break;
    case FrameCodeU64: setBufferSize(8); break;
    default: assert(false); break;
    }
}

std::string FrameCodeItem::frameCodeTypeString() const
{
    return frameCodeTypeString(d->frameCodeType);
}

std::string FrameCodeItem::frameCodeTypeString(FrameCodeType type)
{
    switch (type) {
    case FrameCodeU8: return "u8";
    case FrameCodeU16: return "u16";
    case FrameCodeU32: return "u32";
    case FrameCodeU64: return "u64";
    default: return "?";
    }
}

FrameCodeType FrameCodeItem::stringFrameCodeType(const std::string &str)
{
    typedef std::unordered_map<std::string, int> map_strtype;
    static const map_strtype::value_type map_data[FrameCodeTotal] = {
        map_strtype::value_type("u8", FrameCodeU8),
        map_strtype::value_type("u16", FrameCodeU16),
        map_strtype::value_type("u32", FrameCodeU32),
        map_strtype::value_type("u64", FrameCodeU64)
    };
    static const map_strtype _map(map_data, map_data + FrameCodeTotal);
    map_strtype::const_iterator citer = _map.find(str);
    if (citer == _map.cend()) {
        return FrameCodeInvalid;
    } else {
        return FrameCodeType(citer->second);
    }
}

double FrameCodeItem::data() const
{
    const char *buffer = this->buffer();
    if (!buffer) {
        return 0.0;
    }

    icd_uint64 value = 0;
    memcpy(&value, buffer, std::min<int>(sizeof(value), int(bufferSize())));
    return (double)value;
}

void FrameCodeItem::setData(double data)
{
    char *buffer = this->buffer();
    if (!buffer) {
        return;
    }

    icd_uint64 value = (icd_uint64)data;
    memcpy(buffer, &value, std::min<int>(sizeof(value), int(bufferSize())));
}

std::string FrameCodeItem::dataString() const
{
    std::stringstream str;
    str << std::hex << (icd_uint64)data();
    return str.str();
}

std::string FrameCodeItem::frameId() const
{
    return d->frameId;
}

void FrameCodeItem::setFrameId(const std::string &id)
{
    d->frameId = id;
}

FrameItemPtr FrameCodeItem::frame() const
{
    return d->frame;
}

void FrameCodeItem::setFrame(const FrameItemPtr &frame)
{
    frame->setParent(this);
    d->frame = frame;
}

void FrameCodeItem::updateSend(bool period)
{
    if (!d->frame) {
        return;
    }

    if (period) {
        icd_uint64 code = d->frame->updateSend(0);
        if (code > 0) {
            setData((double)code);
        }
    } else {
        d->frame->updateSend((icd_uint64)data());
    }
}

void FrameCodeItem::updateRecv()
{
    if (!d->frame) {
        return;
    }

    d->frame->updateRecv((icd_uint64)data());
}

void FrameCodeItem::resetSend()
{
    if (!d->frame) {
        return;
    }

    d->frame->resetSend();
}

void FrameCodeItem::resetData()
{
    if (d->frame) {
        d->frame->resetData();
    }
}

void FrameCodeItem::clearData()
{
    if (d->frame) {
        d->frame->clearData();
    }
}

std::string FrameCodeItem::typeName() const
{
    std::stringstream ss;
    ss << "icd_uint" << ((int)bufferSize() << 3);
    return ss.str();
}

std::string FrameCodeItem::typeString() const
{
    return Item::typeString() + "#" + frameCodeTypeString();
}

Object *FrameCodeItem::clone() const
{
    return new FrameCodeItem(*this);
}

FrameCodeItem &FrameCodeItem::operator =(const FrameCodeItem &other)
{
    Item::operator =(other);

    d->frameId = other.d->frameId;

    if (other.d->frame) {
        d->frame = FrameItemPtr(reinterpret_cast<FrameItem *>(other.d->frame->clone()));
    }

    return *this;
}

Json::Value FrameCodeItem::save() const
{
    Json::Value json = Item::save();

    json["frameCodeType"] = frameCodeTypeString();
    if (!d->frameId.empty()) {
        json["frameId"] = d->frameId;
    }

    return json;
}

bool FrameCodeItem::restore(const Json::Value &json, int deep)
{
    if (!Item::restore(json, deep)) {
        return false;
    }

    FrameCodeType frameCodeType = stringFrameCodeType(json["frameCodeType"].asString());
    if (frameCodeType == Icd::FrameCodeInvalid) {
        assert(false);
        return false;
    }
    setFrameCodeType(frameCodeType);
    setFrameId(json["frameId"].asString());

    return true;
}

} // end of namespace Icd
