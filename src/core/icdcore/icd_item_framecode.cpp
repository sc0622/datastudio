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
        : frameCodeType(FrameCodeU8)
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

int FrameCodeItem::rtti() const
{
    return ObjectFrameCode;
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
    default: break;
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
    return dataFromBuffer(buffer());
}

void FrameCodeItem::setData(double data)
{
    char *buffer = this->buffer();
    if (!buffer) {
        return;
    }

    icd_uint64 value = static_cast<icd_uint64>(data);
    memcpy(buffer, &value, std::min<size_t>(sizeof(value), static_cast<size_t>(bufferSize())));
}

std::string FrameCodeItem::dataString() const
{
    std::stringstream str;
    str << std::hex << static_cast<icd_uint64>(data());
    return str.str();
}

double FrameCodeItem::dataFromBuffer(const char *buffer) const
{
    if (!buffer) {
        return 0.0;
    }

    icd_uint64 value = 0;
    memcpy(&value, buffer, std::min<size_t>(sizeof(value), static_cast<size_t>(bufferSize())));
    return static_cast<double>(value);
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
    d->frame = frame;
}

bool FrameCodeItem::isFrameValid() const
{
    return (d->frame != nullptr);
}

void FrameCodeItem::updateSend(bool period)
{
    if (!d->frame) {
        return;
    }

    if (period) {
        icd_uint64 code = d->frame->updateSend(0);
        if (code > 0) {
            setData(static_cast<double>(code));
        }
    } else {
        d->frame->updateSend(static_cast<icd_uint64>(data()));
    }
}

void FrameCodeItem::updateRecv()
{
    if (!d->frame) {
        return;
    }

    d->frame->updateRecv(static_cast<icd_uint64>(data()));
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
    ss << "icd_uint" << (static_cast<int>(bufferSize()) << 3);
    return ss.str();
}

std::string FrameCodeItem::typeString() const
{
    return Item::typeString() + "#" + frameCodeTypeString();
}

ObjectPtr FrameCodeItem::copy() const
{
    return std::make_shared<FrameCodeItem>(*this);
}

ObjectPtr FrameCodeItem::clone() const
{
    FrameCodeItemPtr newFrameCode = std::make_shared<FrameCodeItem>(*this);
    // children
    if (d->frame) {
        FrameItemPtr newFrame = JHandlePtrCast<FrameItem>(d->frame->clone());
        newFrame->setParent(newFrameCode.get());
        newFrameCode->setFrame(newFrame);
    }
    return newFrameCode;
}

FrameCodeItem &FrameCodeItem::operator =(const FrameCodeItem &other)
{
    if (this == &other) {
        return *this;
    }
    Item::operator =(other);
    d->frameCodeType = other.d->frameCodeType;
    d->frameId = other.d->frameId;
    d->frame.reset();
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
        return false;
    }
    setFrameCodeType(frameCodeType);
    setFrameId(json["frameId"].asString());

    return true;
}

} // end of namespace Icd
